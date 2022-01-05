IP Protection with Symmetric Authentication
------------------------
The IP protection can be easily integrated to the existing projects.The user project should include symmetric_authentication.c & symmetric_authentication.h files which contains the api
- **symmetric_authenticate()** - For Performing the authentication between host & device.

User Considerations
-----------
- The user should take care on how the master key should be stored on the MCU side.
- The api's in the file doesn't do the provisioning of the chip and user should take care of the provisioning.

With the provisioned cryptoauthentication device and after doing the cryptoauthlib initialisation,user should only be calling the function symmetric_authenticate() with its necessary parameters for the authentication. The returned authentication status should be used in the application.

Examples
-----------
For more information about IP protection and its example project refer [Microchip github](https://github.com/MicrochipTech)
