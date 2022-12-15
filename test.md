How to setup the Calypso Azure IoT plug and play design module

1.	Create an IoT central application

a.	Sign in to the Azure portal, https://portal.azure.com/
b.	 From the Azure homepage, select the "+ Create a resource button" and then enter "IoT Central application" in the Search the Marketplace field. 
c.	Select "IoT Central application" from the search results and the select "Create".

![image](https://user-images.githubusercontent.com/105643484/207836830-a699de6c-e931-48e7-aa98-7ab5afffc9ed.png)         
Figure 1: IoT central application

     On the Basics tab, complete the fields as follows: 
a.	Subscription: Select the subscription to use for the application.
b.	Resource group: Select a resource group or create a new one. To create a new one, select Create new and fill in the name you want to use. To use an existing re source group, select that resource group. For more information, see Manage Azure Resource Manager resource groups.
c.	Resource name: Type in a name for the IoT central application.
d.	Application URL: This will be automatically set to .azureiotcentral.com 
e.	Template: From the drop down, select "Custom application".
f.	Region: Select the region in which the application will be located. Select a location that is geographically the closest.
g.	Pricing Plan: Choose the appropriate pricing tier. The standard tier 0 is good to start prototyping. More details on pricing can be found under, https://azure.microsoft.com/en-us/pricing/details/iot-central
 ![image](https://user-images.githubusercontent.com/105643484/207836885-8fb1d1e7-5159-4743-b54a-d5a33851599a.png)
Figure 2: IoT central application parameters

h.	Click on "Review + Create". 
i.	In the following page, review the terms and click on "Create". 
j.	Wait for the deploymet to complete. After the process is complete, click on "Go to re source" button to open the application. 
k.	Click on the IoT central application URL to open the newly created IoT central platform. 
l.	In the IoT central app open, "Permissions > Device connection groups" and note down the ID scope parameter for use in futher steps.
![image](https://user-images.githubusercontent.com/105643484/207837683-add0c6da-7a6d-4c4a-af44-b60414e7edf4.png)
Figure 3: IoT central application deployment complete

 ![image](https://user-images.githubusercontent.com/105643484/207837718-22ec3e04-b952-42aa-b445-f3cba0815955.png)
Figure 4: IoT central application URL

 ![image](https://user-images.githubusercontent.com/105643484/207837747-b5510895-abf8-4984-984a-1bca379e3f6a.png)
Figure 5: IoT central application homepage

 ![image](https://user-images.githubusercontent.com/105643484/207837777-1becb6a0-9a6c-4385-a6ec-397d0ee9b1e3.png)
Figure 6: ID scope for the created IoT central application

2.	Create certificates
In order to securely connect the device to IoT central application, the device needs to implement certain method for authentication. In this case, the X.509 certificate based authentication is implemented. This method requires creation of certificates for every device. In order to enable easy prototyping, Würth Elektronik eiSos’s Certificate creation tool can be used. This tool creates all the certificates necessary to get started.

•	Download the WE certificate generator tool from https://www.we-online.com/certificategenerator 
•	Unzip to a suitable location on the computer and open the executable "WECertificateUploader.exe"
Inside the WE Certificate Generator tool, fill in the following fields to generate the required certificates for the cloud service
a.	SSID, SSID Password : Select your WiFi SSID and its password for uploading to the calypso module
b.	Security: Select the appropriate type of Security used in the WiFi
c.	Device ID: This is the name of the device as it appears in the IoT central APP. It needs to be unique per device. 
d.	Scoe ID: Type in the ID scope noted in the previous section. ID scope is unique per application but common across devices. 
e.	NTP server: Enter the time server of choice that the module will use to get the current time.
f.	Time zone: Select the appropriate time zone.
 ![image](https://user-images.githubusercontent.com/105643484/207837821-9db475e7-6ebb-4f52-9320-e52c4951d31e.png)
Figure 7: Parameters to configure the device

•	Device root certificate: This is the self-signed certificate that acts as the root of trust for all devices. The device root certificate is used to generate leaf certificates. Each device has a unique leaf certificate that identifies the device. The root certificate can be generated once and used for generating leaf certificates for several device. This tool allows creation on a new root certificate, saving the same and loading it back for subsequent usage. 
1. On first time use, set the validity time in months 
2. Click on "Create root certificate" to create a new root certificate. 
3. If a root certificate already exists, click on "Load root certificate". This opens a file browser. Browse to the correct location to choose the previously used root certificate. 
4. Click on "Save root certificate" to save the generated root certificate for future use. 
5. Click on "Export root PEM" to export the certificate in PEM format. This file needs to be uploaded to the IoT central application
6. Click on "Display root certificate" to view the certificate in the standard Windows format.

 ![image](https://user-images.githubusercontent.com/105643484/207837851-1ff70a96-48c0-4400-9f57-5bb33a011939.png)
Figure 8: Device root certificate options
•	Device certificate: Every device requires a unique device certificate to securely connect to the IoT central application. Each device certificate generated is exclusively linked to the device through the device ID and cannot be used on any other device. 
1. For every device ID, set the validity time in months 
2. Click on "Create device certificate" to create a new device certificate
3. Click on "Export device PEM" to export the certificate in PEM format. This file needs to be uploaded to the device  
4. Click on "Display device certificate" to view the certificate in the standard Windows format.

 ![image](https://user-images.githubusercontent.com/105643484/207837880-2ee80271-2206-4bba-8590-ecc44fceed02.png)
Figure 9: Device certificate options

•	Device private key: This is the private key corresponding to the public key in the device certificate and is also uniquely linked to a device ID. Click on "Export device key" to export the key in PEM format. This file needs to be uploaded to the device as will be explained in the subsequent sections.
![image](https://user-images.githubusercontent.com/105643484/207837918-fede0f21-6b11-4493-8b5f-da0265531b22.png) 
Fig 10: Device certificate options
       
![image](https://user-images.githubusercontent.com/105643484/207837943-094189d7-afa4-4a41-9601-a4e324c02b0f.png)
Figure 11 : Display of WE Certificate Generator Uploader application


