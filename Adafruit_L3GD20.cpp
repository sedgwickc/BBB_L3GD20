/***************************************************************************
  This library is a port of Adafruit's L3GD20 library for Arduino to the Beagle
  Bone Black using the MRAA library in the place of Wire.h to communicate over I2C.

  This port is written and maintained by Charles Sedgwick. 
  This port retains the licence of the software it is based off of which is
  described below.
 ***************************************************************************
  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Kevin Townsend for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include "Adafruit_L3GD20.hpp"
#include <iostream>

using namespace std;

namespace rover {

/***************************************************************************
 CONSTRUCTOR
 ***************************************************************************/
 
/**************************************************************************/
/*!
    @brief  Instantiates a new Adafruit_L3GD20 class
*/
/**************************************************************************/
Adafruit_L3GD20::Adafruit_L3GD20(unsigned int I2CBus){
	this->i2c_l3gd20 = new mraa::I2c(I2CBus, true);
}

/***************************************************************************
 PUBLIC FUNCTIONS
 ***************************************************************************/

/**************************************************************************/
/*!
    @brief  Setups the HW
*/
/**************************************************************************/

bool Adafruit_L3GD20::begin(l3gd20Range_t rng, uint8_t addr)
{
	this->I2CAddress = addr;
	this->range = rng;

	uint8_t id = readRegister(addr, L3GD20_REGISTER_WHO_AM_I);
	if( (id != L3GD20_ID) && (id != L3GD20H_ID) ){
		return false;
	}
#ifdef DEBUG
	cout<<"begin(): Gyro identified..."<<endl;
#endif

/* Set CTRL_REG1 (0x20)
   ====================================================================
   BIT  Symbol    Description                                   Default
   ---  ------    --------------------------------------------- -------
   7-6  DR1/0     Output data rate                                   00
   5-4  BW1/0     Bandwidth selection                                00
     3  PD        0 = Power-down mode, 1 = normal/sleep mode          0
     2  ZEN       Z-axis enable (0 = disabled, 1 = enabled)           1
     1  YEN       Y-axis enable (0 = disabled, 1 = enabled)           1
     0  XEN       X-axis enable (0 = disabled, 1 = enabled)           1 */

  /* Switch to normal mode and enable all three channels */
  writeRegister(addr,L3GD20_REGISTER_CTRL_REG1, 0x0F);
#ifdef DEBUG
	cout<<"begin(): Gyro configured..."<<endl;
#endif
  /* ------------------------------------------------------------------ */

  /* Set CTRL_REG2 (0x21)
   ====================================================================
   BIT  Symbol    Description                                   Default
   ---  ------    --------------------------------------------- -------
   5-4  HPM1/0    High-pass filter mode selection                    00
   3-0  HPCF3..0  High-pass filter cutoff frequency selection      0000 */

  /* Nothing to do ... keep default values */
  /* ------------------------------------------------------------------ */

  /* Set CTRL_REG3 (0x22)
   ====================================================================
   BIT  Symbol    Description                                   Default
   ---  ------    --------------------------------------------- -------
     7  I1_Int1   Interrupt enable on INT1 (0=disable,1=enable)       0
     6  I1_Boot   Boot status on INT1 (0=disable,1=enable)            0
     5  H-Lactive Interrupt active config on INT1 (0=high,1=low)      0
     4  PP_OD     Push-Pull/Open-Drain (0=PP, 1=OD)                   0
     3  I2_DRDY   Data ready on DRDY/INT2 (0=disable,1=enable)        0
     2  I2_WTM    FIFO wtrmrk int on DRDY/INT2 (0=dsbl,1=enbl)        0
     1  I2_ORun   FIFO overrun int on DRDY/INT2 (0=dsbl,1=enbl)       0
     0  I2_Empty  FIFI empty int on DRDY/INT2 (0=dsbl,1=enbl)         0 */

  /* Nothing to do ... keep default values */
  /* ------------------------------------------------------------------ */

  /* Set CTRL_REG4 (0x23)
   ====================================================================
   BIT  Symbol    Description                                   Default
   ---  ------    --------------------------------------------- -------
     7  BDU       Block Data Update (0=continuous, 1=LSB/MSB)         0
     6  BLE       Big/Little-Endian (0=Data LSB, 1=Data MSB)          0
   5-4  FS1/0     Full scale selection                               00
                                  00 = 250 dps
                                  01 = 500 dps
                                  10 = 2000 dps
                                  11 = 2000 dps
     0  SIM       SPI Mode (0=4-wire, 1=3-wire)                       0 */

  /* Adjust resolution if requested */
  switch(range)
  {
    case L3DS20_RANGE_250DPS:
      writeRegister(addr,L3GD20_REGISTER_CTRL_REG4, 0x00);
      break;
    case L3DS20_RANGE_500DPS:
      writeRegister(addr,L3GD20_REGISTER_CTRL_REG4, 0x10);
      break;
    case L3DS20_RANGE_2000DPS:
      writeRegister(addr,L3GD20_REGISTER_CTRL_REG4, 0x20);
      break;
  }
  /* ------------------------------------------------------------------ */

  /* Set CTRL_REG5 (0x24)
   ====================================================================
   BIT  Symbol    Description                                   Default
   ---  ------    --------------------------------------------- -------
     7  BOOT      Reboot memory content (0=normal, 1=reboot)          0
     6  FIFO_EN   FIFO enable (0=FIFO disable, 1=enable)              0
     4  HPen      High-pass filter enable (0=disable,1=enable)        0
   3-2  INT1_SEL  INT1 Selection config                              00
   1-0  OUT_SEL   Out selection config                               00 */

  /* Nothing to do ... keep default values */
  /* ------------------------------------------------------------------ */

	return true;
}

void Adafruit_L3GD20::cleanup(){
	delete(this->i2c_l3gd20);
}

void Adafruit_L3GD20::readGyro(float *x, float *y, float *z){

	uint8_t xhi, xlo, yhi, ylo, zhi, zlo;

    this->i2c_l3gd20->address(L3GD20_ADDRESS);
    uint8_t retval[MAX_BUFFER_LENGTH];
    this->i2c_l3gd20->writeByte(L3GD20_REGISTER_OUT_X_L | 0x80);
    this->i2c_l3gd20->read(retval, MAX_BUFFER_LENGTH);

    xlo = (uint8_t)retval[0];
    xhi = (uint8_t)retval[1];
    ylo = (uint8_t)retval[2];
    yhi = (uint8_t)retval[3];
    zlo = (uint8_t)retval[4];
    zhi = (uint8_t)retval[5];

	int16_t x_raw = (int16_t)(xlo | (xhi << 8));
	int16_t y_raw = (int16_t)(ylo | (yhi << 8));
	int16_t z_raw = (int16_t)(zlo | (zhi << 8));

	// Compensate values depending on the resolution
	switch(this->range)
	{
	case L3DS20_RANGE_250DPS:
  	  *x = x_raw * L3GD20_SENSITIVITY_250DPS;
  	  *y = y_raw * L3GD20_SENSITIVITY_250DPS;
  	  *z = z_raw * L3GD20_SENSITIVITY_250DPS;
  	  break;
	case L3DS20_RANGE_500DPS:
  	  *x = x_raw * L3GD20_SENSITIVITY_500DPS;
  	  *y = y_raw * L3GD20_SENSITIVITY_500DPS;
  	  *z = z_raw * L3GD20_SENSITIVITY_500DPS;
  	  break;
	case L3DS20_RANGE_2000DPS:
  	  *x = x_raw * L3GD20_SENSITIVITY_2000DPS;
  	  *y = y_raw * L3GD20_SENSITIVITY_2000DPS;
  	  *z = z_raw * L3GD20_SENSITIVITY_2000DPS;
  	  break;
	}
}



/**************************************************************************/
/*!
    @brief  Writes an 8 bit value over I2C
*/
/**************************************************************************/
void Adafruit_L3GD20::writeRegister(unsigned int address,unsigned int reg, unsigned char value)
{
	this->i2c_l3gd20->address(address);
	this->i2c_l3gd20->writeReg( reg, (uint8_t)value );
}

/**************************************************************************/
/*!
    @brief  Reads an 8 bit value over I2C
*/
/**************************************************************************/
uint8_t Adafruit_L3GD20::readRegister(unsigned int address, unsigned int reg)
{
		this->i2c_l3gd20->address(address);
		return (uint8_t)this->i2c_l3gd20->readReg(reg);
}

Adafruit_L3GD20::~Adafruit_L3GD20(){}
}; // rover namespace

