**PVconverter**

1. **PVconverter Quick Introduction**

   &nbsp;&nbsp;&nbsp;PVconverter is a language translation tool which can convert the commonly used physical verification programming languages to each other.The applicable PV languages include x SVRF, PVS, PVRS, etc.

   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;The widely used physical verification tools include caliber, Arugs, PVS which all have the mature function of DRC (Design Rule Check), LVS (Layout Versus Schematic) check, ERC (Electrical Rule Check). The PVconverter is developed for helping the designer to transfer the PV rule-deck among the commonly used PV languages. With the help of the converter, the users will do the verification work smoothly.

   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;This guide contains information about the PVconverter Commands and is intended for developers who need transfer the calibre physical verification rules to Argus/PVS format. This guide does not include information about installing the PVconverter tool.

2. Usage:

Syntax1: svrf2pvrs -i svrf\_rule -o pvrs\_rule

Syntax2: svrf2pvs -i svrf\_rule -o pvs\_rule

Option:

• [-i svrf\_rule\_file]: Specify A required input SVRF ruledeck.

• [-sf switch\_file]: Specify the input switches in this file. Switch file format: #define METAL5

• [-full]: This option instructs converter to transfer all switches in the ruledeck. This option is not recommended because converter may fail to convertor the ruledeck for some situations which you can find in the readme.txt in the converter package. If not use this option, converter only convert the active switches to pvrs ruledeck.

• [-oc]: Output the comments in the ruledeck to PVRS ruledeck.

• [-o[l] pvrs\_rule\_file|pvs\_rule\_file]: Specify the output PVRS ruledeck. If use the &quot;-ol&quot; option, svrf2pvrs will output the SVRF rules to the comments of PVRS ruledeck.

• [-trs]: instructs the convertor convert a TVF rule deck into a TRS rule deck

• [-gui]: Use GUI to setup and run convertor. As Fig.1




![](/images/fig1.png)

<center>Fig.1 PVco nverter Graphical User Interface</center>

Description:

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Converts the Physical-Verification ruledeck among different programming language.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Convertor can&#39;t convert non-SVRF statement.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;There are many switches(#define) in a rule deck, convertor only convert effective path and delete non effective switches and paths by default. If a use changes the switches in SVRS rule deck, PVRS/PVS rue deck should be converted again. [-full] instructs the convertor convert all switches and paths at the same time. In default mode, PVconvertor could transfer all rules which it supports. But in full mode, some special structures could not be transferred without the help of users.

**3. Error Reporting**

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;• PVconvertor can&#39;t convert non-SVRF statement. If there are non-SVRF statement it will be report &quot;svrf error&quot;.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;• If &quot;#IFDEF&quot; or &quot;#ENDIF&quot; is written in comments, PVconvertor will report an error.

If there are unsupported SVRF statements, convertor will report a warning and ignore them.

Unsupported statements include DRC run-time TVF, PERC, PEX, RET, ICTRACE, MASK and OPC (OPC BIAS is supported)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;• PVconvertor only supports 7 DFM rules. They are: DFM COPY, DFM NARAC, DFM PROPERTY, DFM RDB, DFM STAMP, DFM SPACE, DFM SIZE

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;• PVconvertor could not convert a complete SVRF statement if it is separated by &quot;#ifdef#else#endif&quot;. A statement should be completed in each branch. For example. DMACRO defined the property of the devices. It is a part of device statements.The device statement is not complete.

_DMACRO mim\_prop seed\_layer area\_c perimeter\_c {_

_[_

_PROPERTY C_

_A = area(seed\_layer)_

_P = perimeter(seed\_layer)_

_C = A\*area\_c + P\*fringe\_c_

_]_

_#IFDEF METAL4_

_DEVICE C(MIM) MiM\_C MiM\_C(POS) M4(NEG)_

_#ENDIF_

_#IFDEF METAL5{_

_DEVICE C(MIM) MiM\_C MiM\_C(POS) M5(NEG)_

_#ENDIF_

_CMACRO mim\_prop MiM\_C 1.1e-3 1.2e-9_

PVconverter need complete the statement in each branch like this:

_#IFDEF METAL4_

_DEVICE C(MIM) MiM\_C MiM\_C(POS) M4(NEG)_

_CMACRO mim\_prop MiM\_C 1.1e-3 1.2e-9_

_#ENDIF_

_#IFDEF METAL5{_

_DEVICE C(MIM) MiM\_C MiM\_C(POS) M5(NEG)_

_CMACRO mim\_prop MiM\_C 1.1e-3 1.2e-9_

_#ENDIF_

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;• If a macro is not a complete statement, it could not be defined in many branches. For example: In this case, the macro mim\_prop is a part of a device statement and defined in the two branches. PVconvertor will complete the non-complete statement before convert it to PVRS. So an error of duplicate definitions will be reported.

_#IFDEF PROPERTY\_C_

_DMACRO mim\_prop seed\_layer {_

_[_

_PROPERTY C_

_A = area(seed\_layer)_

_P = perimeter(seed\_layer)_

_C = A\*1.1e-3 + P\*1.2e-9_

_]#ELSE_

_DMACRO mim\_prop seed\_layer {_

_[_

_PROPERTY A P_

_A = area(seed\_layer)_

_P = perimeter(seed\_layer)_

_]_

_#ENDIF_

_DEVICE C(MIM) MiM\_C MiM\_C(POS) M5(NEG)_

_CMACRO mim\_prop MiM\_C_

Because the macro name is duplicate, PVconverter need to change the names of the macros and make a branch for the device calling the macro like:

_DMACRO mim\_prop\_c seed\_layer {_

_[_

_PROPERTY C_

_A = area(seed\_layer)_

_P = perimeter(seed\_layer)_

_C = A\*1.1e-3 + P\*1.2e-9_

_]_

_DMACRO mim\_prop\_ap seed\_layer {_

_[_

_PROPERTY A P_

_A = area(seed\_layer)_

_P = perimeter(seed\_layer)_

_]_

_#IFDEF PROPERTY\_C_

_DEVICE C(MIM) MiM\_C MiM\_C(POS) M5(NEG)_

_CMACRO mim\_prop\_c MiM\_C_

_#ELSE_

_DEVICE C(MIM) MiM\_C MiM\_C(POS) M5(NEG)_

_CMACRO mim\_prop\_ap MiM\_C_

_#ENDIF_

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;• If tvf rule file contains the usage of consecutive svrf layer operation keywords more than once, convertor may fail. By default, PVconvertor generates one temporary layer definition for every layer operation command to avoid this case.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;• For unsupported TVF rules, PVconvertor reports warning and skip them.
