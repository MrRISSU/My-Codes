************************************************************
This code gets JSON Timestamp from a Rest API.
And extract the Timestamp to integer.
************************************************************

#############  Hardware   ###################

1. MicrEdge 300

############## Libraries ####################

1. WiFiClientSecure
2. ArduinoJson

################ Root CA ####################

Steps to Obtain the Root CA Certificate in PEM Format

•	Visit the API Link:
	o	Open the following link in your web browser to access the API:
		https://loot5pcssa.execute-api.ap-south-1.amazonaws.com/v1/epochtime

•	Check the Certificate Information:
	o	Click on the padlock icon in the address bar to view the website security information.
	o	Select "Connection is secure" to expand the details.
	o	Click on "Certificate is valid."
	o	In the Certificate window, navigate to the "Details" tab.

•	Copy the Copy the Issuer Field Values:
	o	Within the "Details" tab, you will find various field values. Copy the relevant ones, particularly those pertaining to the Root CA certificate.
	o	Issuer for current API Link:
		CN = Amazon RSA 2048 M02, O = Amazon, C = US

•	Search for the Certificate:
	o	Use the copied information to search online or in your browser for the Root CA certificate associated with the given API or domain.

•	Download the PEM Formatted Certificate From a valid Website:
	o	Use the following link to download the PEM formatted file(for current API Link):
		https://www.e2encrypted.com/certs/414a2060b738c635cc7fc243e052615592830c53/#ca-certificate-in-pem-format


