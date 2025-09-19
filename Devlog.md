##

## 2025-09-19

- Added a new mode to the web server that allows for a two-phase reveal write. Although this currently only works with post requests. Such as:
```bash
curl -X POST http://splitflap.local/reveal \
  -H "Content-Type: application/json" \
  -d '{"text":"MANDA","center":true,"speed":15}'
{"message":"Reveal executed","type":"success"}%    
```
[TODO] I tried to add it to the web server so that it would work with the UI but not succesful yet. Will check out later.