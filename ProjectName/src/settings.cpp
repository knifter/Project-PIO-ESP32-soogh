#include "settings.h"

#include <Arduino.h>
#include <nvs.h>

#include "config.h"
#include "globals.h"
#include "tools-log.h"

SettingsManager::SettingsManager(settings_t& settings) : NVSettings(&settings, sizeof(settings_t))
{

};

bool SettingsManager::set_defaults_since(const uint32_t data_version)
{
    settings_t *settings = static_cast<settings_t*>(_data);

    switch(data_version)
    {
        default:
            ERROR("Unknown data version: %d", data_version);
            return false;
        case 0: // empty blob
            DBG("Init settings v0: defaults");
            memset(_data, 0, _data_size);
            settings->example = 0;

        // End with the current version:
        case 1:
            _data_version = 1;
            return true;
    };

	return true;
};

bool SettingsManager::read_blob(void* blob, const size_t blob_size, const uint32_t blob_version)
{
    switch(blob_version)
    {
        default:
            ERROR("Unknown blob version: %d", blob_version);
            return false;

        case 1: // updateable
        case 2: // valid, up-to-date settings
            memcpy(_data, blob, blob_size);
            set_defaults_since(blob_version);
            _dirty = false;
            return true;
    };

	return true;
};
