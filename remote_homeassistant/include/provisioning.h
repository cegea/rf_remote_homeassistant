#ifndef provisioning_h
#define provisioning_h

/**
 * @brief Setup function for the provisioning application.
 * 
 */
void provisioning_setup(void);

/**
 * @brief Loop function for the provisioning application
 * 
 */
void provisioning_loop(void);

/**
 * @brief Clean credentials by assigning 0 to each values
 * 
 */
void __clean_wifi_credentials(void);

#endif