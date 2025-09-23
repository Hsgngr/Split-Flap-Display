# DEVLOG

## 2025-09-23

- Changed the location of the reboot button to the settings page.
- If you want to test how the website works without uploading the changes, you can use the following command:

```zsh
npx vite --host 
```

## 2025-09-21

- Now the reveal mode works with the UI.
- Don't forget to

```bash
pio run -e esp32_c3_ota -t upload

AND


pio run -e esp32_c3_ota -t uploadfs
```

otherwise the changes might not work.

[TODO]There should be a better way to upload both of these changes without rebooting twice.

[TODO] Check why Time is not working. (Also Date mode.)
[TODO] Add to the Diagnosis, what the Display thinks that it shows. It can help us to debug.
[TODO] The last pin is not working properly. It slides from the right plate's middle circle. We need to find a way to stop that. Probably that's why it's not working.

## 2025-09-19

- Added a new mode to the web server that allows for a two-phase reveal write. Although this currently only works with post requests. Such as:

```bash
curl -X POST http://splitflap.local/reveal \
  -H "Content-Type: application/json" \
  -d '{"text":"MANDA","center":true,"speed":15}'
{"message":"Reveal executed","type":"success"}%    
```

[DONE] I tried to add it to the web server so that it would work with the UI but not succesful yet. Will check out later.
