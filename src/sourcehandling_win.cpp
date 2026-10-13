//------------------------------------------------------------------------
// Flags       : clang-format SMTGSequencer
// Project     : mixkey
// Filename    : sourcehandling_win.cpp
// Created by  : nkoeller, 10/2026
// Description : <Description>
//------------------------------------------------------------------------

#include "sourcehandling.h"
#include <string_view>

// clang-format off
#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <iostream>
#include <functiondiscoverykeys_devpkey.h>
#include <string>
// clang-format on

namespace MixKey {

//---------------------------------------------------------------
#define SAFE_RELEASE(punk)  \
	if ((punk) != NULL)     \
	{                       \
		(punk)->Release (); \
		(punk) = NULL;      \
	}

//---------------------------------------------------------------
#define SAFE_DELETE(punk) \
	if ((punk) != NULL)   \
	{                     \
		delete (punk);    \
		(punk) = NULL;    \
	}

// ------------------------------------------------------------------------------------------------
static std::string convert (const std::wstring& wide_string)
{
	if (wide_string.empty ())
	{
		return "";
	}

	const auto size_needed = WideCharToMultiByte (
	    CP_UTF8, 0, &wide_string.at (0), (int)wide_string.size (), nullptr, 0, nullptr, nullptr);
	if (size_needed <= 0)
	{
		throw std::runtime_error ("WideCharToMultiByte() failed: " + std::to_string (size_needed));
	}

	std::string result (size_needed, 0);
	WideCharToMultiByte (CP_UTF8, 0, &wide_string.at (0), (int)wide_string.size (), &result.at (0),
	                     size_needed, nullptr, nullptr);
	return result;
}

// ------------------------------------------------------------------------------------------------
bool getPropertyKeyStringValue (IMMDevice* pDevice, const PROPERTYKEY pkey, std::wstring& wstrValue)
{
	bool valueFound = false;

	if (pDevice != nullptr)
	{
		HRESULT hr = S_FALSE;
		IPropertyStore* pProps = NULL;
		if ((hr = pDevice->OpenPropertyStore (STGM_READ, &pProps)) == S_OK)
		{
			PROPVARIANT varValue;
			PropVariantInit (&varValue);
			if ((hr = pProps->GetValue (pkey, &varValue)) == S_OK)
			{
				if (varValue.vt == VT_LPWSTR && varValue.pwszVal != nullptr)
				{
					wstrValue = varValue.pwszVal;
					valueFound = true;
				}
			}
			PropVariantClear (&varValue);
		}
		SAFE_RELEASE (pProps);
	}

	return valueFound;
}

// ------------------------------------------------------------------------------------------------
static bool findVirtualAudioEndpoints (const std::wstring driverID, const std::wstring endpointName,
                                       std::wstring& endpointId)
{
	IMMDeviceEnumerator* pEnumerator = nullptr;
	bool virtualAudioEndpointFound = false;

	CoCreateInstance (__uuidof (MMDeviceEnumerator), NULL, CLSCTX_ALL,
	                  __uuidof (IMMDeviceEnumerator), (void**)&pEnumerator);

	if (pEnumerator)
	{
		IMMDeviceCollection* pDevCollection = NULL;
		HRESULT hr =
		    pEnumerator->EnumAudioEndpoints (eCapture, DEVICE_STATE_ACTIVE, &pDevCollection);
		if (hr == S_OK && pDevCollection != nullptr)
		{
			UINT count = 0;
			hr = pDevCollection->GetCount (&count);
			if (hr == S_OK && count > 0)
			{
				for (UINT i = 0; i < count; ++i)
				{
					IMMDevice* pDevice = nullptr;
					HRESULT hr = pDevCollection->Item (i, &pDevice);

					if (hr == S_OK && pDevice != nullptr)
					{
						std::wstring matchingDeviceID;
						bool result = getPropertyKeyStringValue (
						    pDevice, PKEY_Device_MatchingDeviceId, matchingDeviceID);
						if (result && !matchingDeviceID.empty ())
						{
							std::size_t found = matchingDeviceID.find (driverID);
							if (found != std::wstring::npos)
							{
								std::wstring epName;
								result =
								    getPropertyKeyStringValue (pDevice, PKEY_Device_DeviceDesc,
								                               epName); // look for eg. "Stream"
								// result = getPropertyKeyStringValue(pDevice,
								// PKEY_Device_FriendlyName, epName); // look for eg. "Stream
								// (Steinberg MixKey)"
								if (result && !epName.empty ())
								{
									std::size_t found = epName.find (endpointName);
									if (found != std::wstring::npos)
									{
										LPWSTR pDefDevID = nullptr;
										hr = pDevice->GetId (&pDefDevID);
										if (hr == S_OK && pDefDevID != nullptr)
										{
											// Endpoint found !!!
											endpointId = pDefDevID;
											CoTaskMemFree (pDefDevID);

											virtualAudioEndpointFound = true;
										}
									}
								}
							}
						}
						SAFE_RELEASE (pDevice);
					}

					if (virtualAudioEndpointFound)
						break;
				}
			}
			SAFE_RELEASE (pDevCollection);
		}
		SAFE_RELEASE (pEnumerator);
	}

	return virtualAudioEndpointFound;
}

//------------------------------------------------------------------------
std::string findPort ()
{
	CoInitialize (NULL); // if not already done

	std::wstring driverID = L"ROOT\\SmtgVirtualAudioDriver";
	std::wstring endPointName = L"Stream"; // if searching in PKEY_Device_DeviceDesc list
	// std::wstring endPointName = L"Stream (Steinberg MixKey)"; // if searching in
	// PKEY_Device_FriendlyName
	std::wstring endpointId;

	bool result = findVirtualAudioEndpoints (driverID, endPointName, endpointId);
	result = result;

	CoUninitialize (); // if not already done

	return convert (endpointId);
}

//------------------------------------------------------------------------
std::string findStreamDeviceId ()
{
	return findPort ();
}

//------------------------------------------------------------------------
} // namespace MixKey
