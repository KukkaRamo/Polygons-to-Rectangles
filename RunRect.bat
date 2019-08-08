for /F "tokens=*" %%c in (OrigDataCSV2.CSV) do echo %%c
for /F "tokens=*" %%c in (OrigDataCSV2.CSV) do  PgToBoundRtgl %%c >> output.CSV
