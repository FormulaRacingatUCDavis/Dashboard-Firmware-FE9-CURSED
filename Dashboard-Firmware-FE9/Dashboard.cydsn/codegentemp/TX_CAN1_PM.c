/*******************************************************************************
* File Name: TX_CAN1.c  
* Version 2.20
*
* Description:
*  This file contains APIs to set up the Pins component for low power modes.
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "TX_CAN1.h"

static TX_CAN1_BACKUP_STRUCT  TX_CAN1_backup = {0u, 0u, 0u};


/*******************************************************************************
* Function Name: TX_CAN1_Sleep
****************************************************************************//**
*
* \brief Stores the pin configuration and prepares the pin for entering chip 
*  deep-sleep/hibernate modes. This function applies only to SIO and USBIO pins.
*  It should not be called for GPIO or GPIO_OVT pins.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None 
*  
* \sideeffect
*  For SIO pins, this function configures the pin input threshold to CMOS and
*  drive level to Vddio. This is needed for SIO pins when in device 
*  deep-sleep/hibernate modes.
*
* \funcusage
*  \snippet TX_CAN1_SUT.c usage_TX_CAN1_Sleep_Wakeup
*******************************************************************************/
void TX_CAN1_Sleep(void)
{
    #if defined(TX_CAN1__PC)
        TX_CAN1_backup.pcState = TX_CAN1_PC;
    #else
        #if (CY_PSOC4_4200L)
            /* Save the regulator state and put the PHY into suspend mode */
            TX_CAN1_backup.usbState = TX_CAN1_CR1_REG;
            TX_CAN1_USB_POWER_REG |= TX_CAN1_USBIO_ENTER_SLEEP;
            TX_CAN1_CR1_REG &= TX_CAN1_USBIO_CR1_OFF;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(TX_CAN1__SIO)
        TX_CAN1_backup.sioState = TX_CAN1_SIO_REG;
        /* SIO requires unregulated output buffer and single ended input buffer */
        TX_CAN1_SIO_REG &= (uint32)(~TX_CAN1_SIO_LPM_MASK);
    #endif  
}


/*******************************************************************************
* Function Name: TX_CAN1_Wakeup
****************************************************************************//**
*
* \brief Restores the pin configuration that was saved during Pin_Sleep(). This 
* function applies only to SIO and USBIO pins. It should not be called for
* GPIO or GPIO_OVT pins.
*
* For USBIO pins, the wakeup is only triggered for falling edge interrupts.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None
*  
* \funcusage
*  Refer to TX_CAN1_Sleep() for an example usage.
*******************************************************************************/
void TX_CAN1_Wakeup(void)
{
    #if defined(TX_CAN1__PC)
        TX_CAN1_PC = TX_CAN1_backup.pcState;
    #else
        #if (CY_PSOC4_4200L)
            /* Restore the regulator state and come out of suspend mode */
            TX_CAN1_USB_POWER_REG &= TX_CAN1_USBIO_EXIT_SLEEP_PH1;
            TX_CAN1_CR1_REG = TX_CAN1_backup.usbState;
            TX_CAN1_USB_POWER_REG &= TX_CAN1_USBIO_EXIT_SLEEP_PH2;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(TX_CAN1__SIO)
        TX_CAN1_SIO_REG = TX_CAN1_backup.sioState;
    #endif
}


/* [] END OF FILE */
