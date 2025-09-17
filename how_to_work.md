# How to work

To work with the display, make sure you have the following:

On the platformio.ini file, you should have Wifi name and password.
also OTA password.

If the OTA is matched with the splitflap.local, you can use the OTA to update the firmware and filesystem.

```zsh
npm run assets 
pio run -e esp32_c3_ota -t uploadfs
```

This will update the filesystem and firmware. how refreshing.

Make sure you use incognito mode to access the settings page so it shows you the correct version.