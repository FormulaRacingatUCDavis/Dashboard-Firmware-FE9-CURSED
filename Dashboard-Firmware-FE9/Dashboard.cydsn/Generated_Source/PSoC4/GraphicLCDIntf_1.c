/*******************************************************************************
* File Name: GraphicLCDIntf_1.c  
* Version 1.80
*
* Description:
*  This file contains the setup, control and status commands for the 
*  GraphicLCDIntf component.   
*
*******************************************************************************
* Copyright 2008-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#include "GraphicLCDIntf_1.h"  

uint8 GraphicLCDIntf_1_initVar = 0u;


/*******************************************************************************
* Function Name: GraphicLCDIntf_1_Enable
********************************************************************************
*
* Summary:
*  There is no need to reset or enable this component.  This component is 
*  naturally a slave that waits for the API calls to perform any 
*  function and after a request is made that request will always complete in 
*  finite time as long as a clock is provided. The only thing provided
*  by this API is setting the Command FIFO to drive at least the half-empty status
*  from the FIFO.
*
*******************************************************************************/
void GraphicLCDIntf_1_Enable(void) 
{   
    /* Nothing to enable */
}


/*******************************************************************************
* Function Name: GraphicLCDIntf_1_Init
********************************************************************************
*
* Summary:
*  Inits/Restores default GraphicLCDIntf configuration provided with the customizer.
* 
*******************************************************************************/
void GraphicLCDIntf_1_Init(void) 
{
    /* Set low and high pulse widths for a read transaction */
    GraphicLCDIntf_1_READ_LO_PULSE_REG = GraphicLCDIntf_1_READ_LOW_PULSE;
    GraphicLCDIntf_1_READ_HI_PULSE_REG = GraphicLCDIntf_1_READ_HIGH_PULSE;
}


/*******************************************************************************
* Function Name: GraphicLCDIntf_1_Start
********************************************************************************
*
* Summary:
*  Starts the GraphicLCDIntf interface.
*
* Global Variables:
*  GraphicLCDIntf_1_initVar - Used to check the initial configuration, modified on 
*  the first function call.
*
*******************************************************************************/
void GraphicLCDIntf_1_Start(void) 
{
    if (GraphicLCDIntf_1_initVar == 0u)
    {
        GraphicLCDIntf_1_Init();
        GraphicLCDIntf_1_initVar = 1u;
    }
      
    GraphicLCDIntf_1_Enable();
}


/*******************************************************************************
* Function Name: GraphicLCDIntf_1_Stop
********************************************************************************
*
* Summary:
*  Disables the GraphicLCDIntf interface.
*
*******************************************************************************/
void GraphicLCDIntf_1_Stop(void) 
{
    /* Nothing to stop */
}


/*******************************************************************************
* Function Name: GraphicLCDIntf_1_Write16
********************************************************************************
*
* Summary:
*  Initiates a write transaction on the 16-bit parallel
*  interface.
*  The write is a posted write, so this function will return before the write 
*  has actually completed on the interface. If the command queue is full, this 
*  function will not return until space is available to queue this write request.
*
* Parameters:
*  d_c:  Data(1) or Command(0) indication. Passed to the d_c pin.
*
*  wrData: Data sent to the do_msb[7:0] (most significant byte) 
*          and do_lsb[7:0] (least significant byte) pins. do_msb[7:0]
*          presents only for the 16-bit interface.
*
*******************************************************************************/
void GraphicLCDIntf_1_Write16(uint8 d_c, uint16 wrData) 
{
    while((GraphicLCDIntf_1_STATUS_REG & GraphicLCDIntf_1_CMD_QUEUE_FULL) != 0u)
    {
        /* The command queue is full */
    }   
    GraphicLCDIntf_1_CMD_FIFO_REG = d_c;
    #if (GraphicLCDIntf_1_BUS_WIDTH == 16u)
        CY_SET_REG16(GraphicLCDIntf_1_DATA_FIFO_PTR, wrData);
    #else /* 8-bit interface */
        GraphicLCDIntf_1_DATA_FIFO_REG = wrData;
    #endif /* GraphicLCDIntf_1_BUS_WIDTH == 16u */
}
	

/*******************************************************************************
* Function Name: GraphicLCDIntf_1_WriteM16
********************************************************************************
*
* Summary:
*  Initiates multiple write transactions on the 16-bit 
*  parallel interface. The write is a posted write, so this function will return
*  before the write has actually completed on the interface. If the command 
*  queue is full, this function will not return until space is available to queue
*  this write request.
*
* Parameters:
*  d_c:    The Data(1) or Command(0) indication. Passed to the d_c pin.
*
*  wrData: The pointer to the the first element of a data buffer sent to the do_msb[7:0]
*          (most significant byte) and do_lsb[7:0] (least significant byte) pins. 
*          do_msb[7:0] presents only for the 16-bit interface.
*
*  num:    The number of words/bytes to write.
*
*******************************************************************************/
void GraphicLCDIntf_1_WriteM16(uint8 d_c, uint16 wrData[], uint16 num) 
{
    uint32 i;
    
    for(i = 0u; i < num; i++)
    {
        while((GraphicLCDIntf_1_STATUS_REG & GraphicLCDIntf_1_CMD_QUEUE_FULL) != 0u)
        {
            /* The command queue is full */
        }   
        GraphicLCDIntf_1_CMD_FIFO_REG = d_c;  
 
        #if (GraphicLCDIntf_1_BUS_WIDTH == 16u)
            CY_SET_REG16(GraphicLCDIntf_1_DATA_FIFO_PTR, wrData[i]);
        #else /* 8-bit interface */
            GraphicLCDIntf_1_DATA_FIFO_REG = wrData[i];
        #endif /* GraphicLCDIntf_1_BUS_WIDTH == 16u */
    }
}


/*******************************************************************************
* Function Name: GraphicLCDIntf_1_Read16
********************************************************************************
*
* Summary:
*  Initiates a read transaction on the 16-bit parallel interface.
*  The read will execute after all currently posted writes have completed. This 
*  function will wait until the read completes and then returns the read value.
*
* Parameters:
*  d_c: Data(1) or Command(0) indication. Passed to the d_c pin.
*
* Return:
*  The 16-bit read value from the di_msb[7:0] (most significant byte) and 
*  di_lsb[7:0] (least significant byte) pins for the 16-bit interface or the 
*  8-bit read value from the di_lsb[7:0] for 8-bit interface.
*
*******************************************************************************/
uint16 GraphicLCDIntf_1_Read16(uint8 d_c) 
{
    while((GraphicLCDIntf_1_STATUS_REG & GraphicLCDIntf_1_CMD_QUEUE_FULL) != 0u)
    {
        /* The command queue is full */
    }   
    GraphicLCDIntf_1_CMD_FIFO_REG = d_c | GraphicLCDIntf_1_READ_COMMAND;
    
    while((GraphicLCDIntf_1_STATUS_REG & GraphicLCDIntf_1_DATA_VALID) != GraphicLCDIntf_1_DATA_VALID)
    {
        /* Wait until input data is valid */
    }
    
    #if (GraphicLCDIntf_1_BUS_WIDTH == 8u)  /* 8-bit interface */
        return (GraphicLCDIntf_1_DIN_LSB_DATA_REG);
    #else   /* 16-bit interface */
        return (uint16)((uint16)GraphicLCDIntf_1_DIN_MSB_DATA_REG << 8u) | GraphicLCDIntf_1_DIN_LSB_DATA_REG; 
    #endif /* GraphicLCDIntf_1_BUS_WIDTH == 8u */
}


/*******************************************************************************
* Function Name: GraphicLCDIntf_1_ReadM16
********************************************************************************
*
* Summary:
*  Initiates multiple read transactions on the 16-bit 
*  parallel interface. The read will execute after all currently posted writes 
*  have completed. This function will wait until the read completes and then 
*  returns the read value.
*
* Parameters:
*  d_c:    The Data(1) or Command(0) indication. Passed to the d_c pin.
*
*  rdData: The pointer to the the first element of a data buffer read from the do_msb[7:0]
*          (most significant byte) and do_lsb[7:0] (least significant byte) pins. 
*          do_msb[7:0] presents only for the 16-bit interface.
*
*  num:    The number of words/bytes to read.
*
*******************************************************************************/
void GraphicLCDIntf_1_ReadM16(uint8 d_c, uint16 rdData[], uint16 num) 
{
    uint32 i;
    
    for(i = 0u; i < num; i++)
    {
        rdData[i] = GraphicLCDIntf_1_Read16(d_c);
    }
}


/*******************************************************************************
* Function Name: GraphicLCDIntf_1_Write16_A0
********************************************************************************
*
* Summary:
*  Initiates a write transaction on the 16-bit parallel
*  interface with the d_c pin set to 0.
*  The write is a posted write, so this function will return before the write 
*  has actually completed on the interface. If the command queue is full, this 
*  function will not return until space is availale to queue this write request.
*
* Parameters:
*  wrData: Data sent on the do_msb[7:0] (most significant byte) 
*         and do_lsb[7:0] (least significant byte) pins. do_msb[7:0]
*         presents only for 16-bit interface.
*
*******************************************************************************/
void GraphicLCDIntf_1_Write16_A0(uint16 wrData) 
{
    while((GraphicLCDIntf_1_STATUS_REG & GraphicLCDIntf_1_CMD_QUEUE_FULL) != 0u)
    {
        /* Command queue is full */
    }   
    GraphicLCDIntf_1_CMD_FIFO_REG = 0u;
    #if (GraphicLCDIntf_1_BUS_WIDTH == 16u)
        CY_SET_REG16(GraphicLCDIntf_1_DATA_FIFO_PTR, wrData);
    #else /* 8-bit interface */
        GraphicLCDIntf_1_DATA_FIFO_REG = wrData;
    #endif /* GraphicLCDIntf_1_BUS_WIDTH == 16u */
}


/*******************************************************************************
* Function Name: GraphicLCDIntf_1_Write16_A1
********************************************************************************
*
* Summary:
*  Initiates a write transaction on the 16-bit 
*  parallel interface with the d_c pin set to 1. The write is a posted write, 
*  so this function will return before the write has actually completed on the 
*  interface. If the command queue is full, this function will not return until
*  space is availale to queue this write request.
*
* Parameters:
*  wrData: Data sent on the do_msb[7:0] (most significant byte) 
*          and do_lsb[7:0] (least significant byte) pins. do_msb[7:0]
*          presents only for 16-bit interface.
*
*******************************************************************************/
void GraphicLCDIntf_1_Write16_A1(uint16 wrData) 
{
    while((GraphicLCDIntf_1_STATUS_REG & GraphicLCDIntf_1_CMD_QUEUE_FULL) != 0u)
    {
        /* Command queue is full */
    }   
    GraphicLCDIntf_1_CMD_FIFO_REG = 1u;
    #if (GraphicLCDIntf_1_BUS_WIDTH == 16u)
        CY_SET_REG16(GraphicLCDIntf_1_DATA_FIFO_PTR, wrData);
    #else /* 8-bit interface */
        GraphicLCDIntf_1_DATA_FIFO_REG = wrData;
    #endif /* GraphicLCDIntf_1_BUS_WIDTH == 16u */
}


/*******************************************************************************
* Function Name: GraphicLCDIntf_1_WriteM16_A0
********************************************************************************
*
* Summary:
*  Initiates multiple write transactions on the 16-bit 
*  parallel interface with the d_c pin set to 0. The write is a posted write, 
*  so this function will return before the write has actually completed on the 
*  interface. If the command queue is full, this function will not return until
*  space is availale to queue this write request.
*
* Parameters:
*  wrData: Pointer to the the first element of a data buffer sent on the 
*          do_msb[7:0] (most significant byte) and do_lsb[7:0] (least significant
*          byte) pins. do_msb[7:0] presents only for 16-bit interface.
*
*  num:  Number of words/bytes to write.
*
*******************************************************************************/
void GraphicLCDIntf_1_WriteM16_A0(uint16 wrData[], int num) 
{
    GraphicLCDIntf_1_WriteM16(0u, wrData, (uint16)num);
}


/*******************************************************************************
* Function Name: GraphicLCDIntf_1_WriteM16_A1
********************************************************************************
*
* Summary:
*  Initiates multiple data write transactions on the 16-bit 
*  parallel interface with the d_c pin set to 1. The write is a posted write, 
*  so this function will return before the write has actually completed on the 
*  interface. If the command queue is full, this function will not return until
*  space is availale to queue this write request.
*
* Parameters:
*  wrData: Pointer to the the first element of a data buffer sent on the 
*          do_msb[7:0] (most significant byte) and do_lsb[7:0] (least significant
*          byte) pins. do_msb[7:0] presents only for 16-bit interface.
*
*  num:  Number of words/bytes to write.
*
*******************************************************************************/
void GraphicLCDIntf_1_WriteM16_A1(uint16 wrData[], int num) 
{
    GraphicLCDIntf_1_WriteM16(1u, wrData, (uint16)num);
}


/*******************************************************************************
* Function Name: GraphicLCDIntf_1_Read16_A1
********************************************************************************
*
* Summary:
*  Initiates data read transaction on the 16-bit 
*  parallel interface with the d_c pin set to 1. The read will execute after 
*  all currently posted writes have completed. This function will wait until 
*  the read completes and then returns the read value.
*
* Return:
*  The 16-bit read value from the di_msb[7:0] (most significant byte) and 
*  di_lsb[7:0] (least significant byte) pins for the 16-bit interface or the 
*  8-bit read value from the di_lsb[7:0] for 8-bit interface.
*
*******************************************************************************/
uint16 GraphicLCDIntf_1_Read16_A1(void) 
{
    return GraphicLCDIntf_1_Read16(1u);
}


/*******************************************************************************
* Function Name: GraphicLCDIntf_1_ReadM16_A1
********************************************************************************
*
* Summary:
*  Initiates multiple data read transactions on the 16-bit 
*  parallel interface with the d_c pin set to 1. The read will execute after all 
*  currently posted writes have completed. This function will wait until the 
*  read completes and then returns the read value.
*
* Parameters:
*  rdData: Pointer to the the first element of a data buffer read from the do_msb[7:0]
*        (most significant byte) and do_lsb[7:0] (least significant byte) pins. 
*        do_msb[7:0] presents only for 16-bit interface.
*
*  num:  Number of words/bytes to read.
*
*******************************************************************************/
void GraphicLCDIntf_1_ReadM16_A1(uint16 rdData[], int num) 
{
    GraphicLCDIntf_1_ReadM16(1u, rdData, (uint16)num);
}


/* [] END OF FILE */
