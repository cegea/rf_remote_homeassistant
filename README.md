# rf_remote_homeassistant
RF remote based on RP2040 and CC1101. For those sub-gigahertz remotes laying around. 

<!-----

You have some errors, warnings, or alerts. If you are using reckless mode, turn it off to see inline alerts.
* ERRORs: 0
* WARNINGs: 0
* ALERTS: 5

Conversion time: 1.344 seconds.


Using this Markdown file:

1. Paste this output into your source file.
2. See the notes and action items below regarding this conversion run.
3. Check the rendered output (headings, lists, code blocks, tables) for proper
   formatting and use a linkchecker before you publish this page.

Conversion notes:

* Docs to Markdown version 1.0β34
* Thu Oct 19 2023 10:52:10 GMT-0700 (PDT)
* Source doc: hacking_remote_plug_en
* Tables are currently converted to HTML tables.
* This document has images: check for >>>>>  gd2md-html alert:  inline image link in generated source and store images to your server. NOTE: Images in exported zip file from Google Docs may not appear in  the same order as they do in your doc. Please check the images!

----->


<p style="color: red; font-weight: bold">>>>>>  gd2md-html alert:  ERRORs: 0; WARNINGs: 0; ALERTS: 5.</p>
<ul style="color: red; font-weight: bold"><li>See top comment block for details on ERRORs and WARNINGs. <li>In the converted Markdown or HTML, search for inline alerts that start with >>>>>  gd2md-html alert:  for specific instances that need correction.</ul>

<p style="color: red; font-weight: bold">Links to alert messages:</p><a href="#gdcalert1">alert1</a>
<a href="#gdcalert2">alert2</a>
<a href="#gdcalert3">alert3</a>
<a href="#gdcalert4">alert4</a>
<a href="#gdcalert5">alert5</a>

<p style="color: red; font-weight: bold">>>>>> PLEASE check and correct alert issues and delete this message and the inline alerts.<hr></p>


Opening the remote control reveals that it has the integrated HS2260A, which is a clone of the PT2260.

The protocol used in wireless communications is explained in the latter's datasheet. In this case being at the frequency 433.92 MHz.

The format of the transmitted message is as follows:



<p id="gdcalert1" ><span style="color: red; font-weight: bold">>>>>>  gd2md-html alert: inline image link here (to images/image1.png). Store image on your image server and adjust path/filename/extension if necessary. </span><br>(<a href="#">Back to top</a>)(<a href="#gdcalert2">Next alert</a>)<br><span style="color: red; font-weight: bold">>>>>> </span></p>


![alt_text](images/image1.png "image_tooltip")


As seen in the figure, the address is sent first, then the data and then a synchronization bit.

In the case of the command used in that case, the integrated one has the R4 termination, which indicates that it has 8 addressing bits, 4 data bits and one synchronization bit.

The symbolic representation of the address and data bits is according to the following figure:



<p id="gdcalert2" ><span style="color: red; font-weight: bold">>>>>>  gd2md-html alert: inline image link here (to images/image2.png). Store image on your image server and adjust path/filename/extension if necessary. </span><br>(<a href="#">Back to top</a>)(<a href="#gdcalert3">Next alert</a>)<br><span style="color: red; font-weight: bold">>>>>> </span></p>


![alt_text](images/image2.png "image_tooltip")


The synchronization bit is as follows:



<p id="gdcalert3" ><span style="color: red; font-weight: bold">>>>>>  gd2md-html alert: inline image link here (to images/image3.png). Store image on your image server and adjust path/filename/extension if necessary. </span><br>(<a href="#">Back to top</a>)(<a href="#gdcalert4">Next alert</a>)<br><span style="color: red; font-weight: bold">>>>>> </span></p>


![alt_text](images/image3.png "image_tooltip")





# DECODING MESSAGES

Using URH ([https://github.com/jopohl/urh](https://github.com/jopohl/urh)) the messages sent for each of the two operating buttons on the controller are decoded:

Off Button:



<p id="gdcalert4" ><span style="color: red; font-weight: bold">>>>>>  gd2md-html alert: inline image link here (to images/image4.jpg). Store image on your image server and adjust path/filename/extension if necessary. </span><br>(<a href="#">Back to top</a>)(<a href="#gdcalert5">Next alert</a>)<br><span style="color: red; font-weight: bold">>>>>> </span></p>


![alt_text](images/image4.jpg "image_tooltip")


On button:



<p id="gdcalert5" ><span style="color: red; font-weight: bold">>>>>>  gd2md-html alert: inline image link here (to images/image5.jpg). Store image on your image server and adjust path/filename/extension if necessary. </span><br>(<a href="#">Back to top</a>)(<a href="#gdcalert6">Next alert</a>)<br><span style="color: red; font-weight: bold">>>>>> </span></p>


![alt_text](images/image5.jpg "image_tooltip")


The smallest width is about 300 us. Which corresponds to one eighth of the bit width. With which:



* ‘f’  → 10001110 → 0x8e
* ‘0’ → 10001000 → 0x88
* ‘1’ → 11101110 → 0xee
* sync → 10000000 → 0x80

The sequences would be as follows (in hex):


<table>
  <tr>
   <td>A0
   </td>
   <td>A1
   </td>
   <td>A2
   </td>
   <td>A3
   </td>
   <td>A4
   </td>
   <td>A5
   </td>
   <td>A6
   </td>
   <td>A7
   </td>
   <td>D3
   </td>
   <td>D2
   </td>
   <td>D1
   </td>
   <td>D0
   </td>
   <td>SYN
   </td>
  </tr>
  <tr>
   <td colspan="13" >Off Button
   </td>
  </tr>
  <tr>
   <td>8th
   </td>
   <td>8th
   </td>
   <td>8th
   </td>
   <td>8th
   </td>
   <td>8th
   </td>
   <td>88
   </td>
   <td>8th
   </td>
   <td>8th
   </td>
   <td>88
   </td>
   <td>yes
   </td>
   <td>yes
   </td>
   <td>88
   </td>
   <td>80
   </td>
  </tr>
  <tr>
   <td colspan="13" >On button
   </td>
  </tr>
  <tr>
   <td>8th
   </td>
   <td>8th
   </td>
   <td>8th
   </td>
   <td>8th
   </td>
   <td>8th
   </td>
   <td>88
   </td>
   <td>8th
   </td>
   <td>8th
   </td>
   <td>88
   </td>
   <td>yes
   </td>
   <td>88
   </td>
   <td>yes
   </td>
   <td>80
   </td>
  </tr>
</table>


The silences between messages are approximately 5 ms, which with a resolution of 300 us corresponds to 16.6 zeros in binary or 4 bytes with a value of 0 (more are left for convenience).


# CC1101-TOOL

To configure and use this tool, the following commands are used:

setmodulation 2

ON button

Repeatedly:

addraw 0000000008e8e8e8e8e888e8e88ee88ee8000000

OOFF button

Repeatedly:

addraw 0000000008e8e8e8e8e888e8e88eeee888000000

playraw 300

