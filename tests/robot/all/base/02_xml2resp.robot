
*** Settings ***

Library  Process
Library  Support


*** Test Cases ***

File
    Prepare  base/xml2resp/file  base  station-1.xml
    Run process  xml2resp  -o  RESP  station-1.xml
    Count and compare target files text

Pipe
    Prepare  base/xml2resp/pipe  base  station-1.xml
    Run process  xml2resp < station-1.xml > RESP  shell=True
    Count and compare target files text
