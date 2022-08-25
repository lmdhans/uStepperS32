#include "TMC5130.h"

TMC5130::TMC5130()
{
	
}

void TMC5130::init(Spi *handle)
{
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* Peripheral clock enable */
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	
	this->spiHandle = handle;
	
	GPIO_InitStruct.Pin = LL_GPIO_PIN_12;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIOB->BSRR |= LL_GPIO_PIN_12; //Set cs pin high

	GPIO_InitStruct.Pin = LL_GPIO_PIN_1;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIOA->BSRR |= LL_GPIO_PIN_1 << 16; //Set EN low

	this->reset();

	/* Set motor current */
	this->writeRegister(IHOLD_IRUN, IHOLD(this->holdCurrent) | IRUN(this->current) | IHOLDDELAY(this->holdDelay));

	this->enableStealth();
	
	/* Set all-round chopper configuration */
	this->writeRegister(CHOPCONF, TOFF(2) | TBL(2) | HSTRT_TFD(4) | HEND(0));

	/* Set startup ramp mode */
	this->setRampMode(POSITIONING_MODE);

	/* Reset position */
	this->writeRegister(XACTUAL, 0);
	this->writeRegister(XTARGET, 0);

	this->setDeceleration(0xFFFE);
	this->setAcceleration(0xFFFE);

	this->stop();

	while (this->readRegister(VACTUAL) != 0);
	//GPIOA->BSRR |= LL_GPIO_PIN_1 << 16; //Set EN low
}

void TMC5130::setDirection(bool direction)
{
	this->mode = DRIVER_VELOCITY;
	if (direction)
	{
		this->writeRegister(RAMPMODE, VELOCITY_MODE_POS);
	}
	else
	{
		this->writeRegister(RAMPMODE, VELOCITY_MODE_NEG);
	}
}
void TMC5130::setRPM(float rpm)
{
	int32_t velocityDir = rpmToVelocity * rpm;

	if (velocityDir > 0)
	{
		this->setDirection(1);
	}
	else
	{
		this->setDirection(0);
	}

	// The velocity cannot be signed
	uint32_t velocity = abs(velocityDir);

	this->setVelocity((uint32_t)velocity);
}

void TMC5130::reset(void)
{

	// Reset stallguard
	this->writeRegister(TCOOLTHRS, 0);
	this->writeRegister(THIGH, 0);
	this->writeRegister(COOLCONF, 0);
	this->writeRegister(SW_MODE, 0);
	this->clearStall();

	this->writeRegister(XACTUAL, 0);
	this->writeRegister(XTARGET, 0);

	this->writeRegister(IHOLD_IRUN, 0);
	this->writeRegister(CHOPCONF, 0);
	this->writeRegister(GCONF, 0);
	this->writeRegister(PWMCONF, 0);
	this->writeRegister(TPWMTHRS, 0);

	this->writeRegister(RAMPMODE, 0);
	this->writeRegister(VSTART, 0);
	this->writeRegister(A1, 0);
	this->writeRegister(V1, 0);
	this->writeRegister(AMAX, 0);
	this->writeRegister(VMAX, 0);
	this->writeRegister(D1, 0);
	this->writeRegister(VSTOP, 0);
}

void TMC5130::enableStealth()
{
	/* Set GCONF and enable stealthChop */
	this->writeRegister(GCONF, EN_PWM_MODE(1) | I_SCALE_ANALOG(1));
	this->setShaftDirection(0);

	/* Set PWMCONF for StealthChop */
	this->writeRegister(PWMCONF, PWM_AUTOSCALE(1) | PWM_GRAD(1) | PWM_AMPL(128) | PWM_FREQ(0) | FREEWHEEL(2));

	/* Specifies the upper velocity (lower time delay) for operation in stealthChop voltage PWM mode */
	this->writeRegister(TPWMTHRS, 5000);
}

void TMC5130::setShaftDirection(bool direction)
{
	// Read the register to save the settings
	int32_t value = this->readRegister(GCONF);
	// Update the direction bit
	if (direction == 1)
	{
		value |= (0x01 << 4);
	}
	else
	{
		value &= ~(0x01 << 4);
	}
	this->writeRegister(GCONF, value);
}

void TMC5130::setRampMode(uint8_t mode)
{

	switch (mode)
	{
	case POSITIONING_MODE:
		// Positioning mode
		this->writeRegister(VSTART_REG, this->VSTART);
		this->writeRegister(A1_REG, this->A1);
		this->writeRegister(V1_REG, this->V1);
		this->writeRegister(AMAX_REG, this->AMAX);
		this->writeRegister(VMAX_REG, this->VMAX);
		this->writeRegister(DMAX_REG, this->DMAX);
		this->writeRegister(D1_REG, this->D1);
		this->writeRegister(VSTOP_REG, this->VSTOP);	 /* Minimum 10 in POSITIONING_MODE */
		this->writeRegister(RAMPMODE, POSITIONING_MODE); /* RAMPMODE = POSITIONING_MODE */
		break;

	case VELOCITY_MODE_POS:
		// Velocity mode (only AMAX and VMAX is used)
		this->writeRegister(VSTART_REG, this->VSTART);
		this->writeRegister(A1_REG, 0);
		this->writeRegister(V1_REG, 0);
		this->writeRegister(AMAX_REG, this->AMAX);
		this->writeRegister(VMAX_REG, this->VMAX);
		this->writeRegister(DMAX_REG, 0);
		this->writeRegister(D1_REG, 0);
		this->writeRegister(VSTOP_REG, 0);
		this->writeRegister(RAMPMODE, VELOCITY_MODE_POS); /* RAMPMODE = VELOCITY_MODE_POS */
		break;
	}
}

void TMC5130::setAcceleration(uint32_t acceleration)
{
	this->AMAX = acceleration;

	if (this->AMAX > 0xFFFE)
	{
		this->AMAX = 0xFFFE;
	}

	this->writeRegister(AMAX_REG, this->AMAX);
}

void TMC5130::setDeceleration(uint32_t deceleration)
{
	this->DMAX = deceleration;

	if (this->DMAX > 0xFFFE)
	{
		this->DMAX = 0xFFFE;
	}

	this->writeRegister(DMAX_REG, this->DMAX);
}

void TMC5130::stop(void)
{
	this->mode = DRIVER_STOP;
	this->setVelocity(0);
}

void TMC5130::setVelocity(uint32_t velocity)
{
	this->VMAX = velocity;

	if (this->VMAX > 0x7FFE00)
	{
		this->VMAX = 0x7FFE00;
	}

	this->writeRegister(VMAX_REG, this->VMAX);
}

void TMC5130::clearStall(void)
{
	// Reading the RAMP_STAT register clears the stallguard flag, telling the driver to continue.
	this->readRegister(RAMP_STAT);
}

int32_t TMC5130::writeRegister(uint8_t address, uint32_t datagram)
{

	// Enable SPI mode 3 to use TMC5130
	LL_SPI_SetClockPhase(SPI2, LL_SPI_PHASE_2EDGE);
	LL_SPI_SetClockPolarity(SPI2, LL_SPI_POLARITY_HIGH);

	uint32_t package = 0;

	// Add the value of WRITE_ACCESS to enable register write
	address += 0x80;

	GPIOB->BSRR |= LL_GPIO_PIN_12 << 16;  //Set CS low

	this->status = this->spiHandle->transmit8BitData(address);

	package |= this->spiHandle->transmit8BitData((datagram >> 24) & 0xff);
	package <<= 8;
	package |= this->spiHandle->transmit8BitData((datagram >> 16) & 0xff);
	package <<= 8;
	package |= this->spiHandle->transmit8BitData((datagram >> 8) & 0xff);
	package <<= 8;
	package |= this->spiHandle->transmit8BitData((datagram)&0xff);

	GPIOB->BSRR |= LL_GPIO_PIN_12; //Set cs pin high

	return package;
}

int32_t TMC5130::readRegister(uint8_t address)
{

	// Enable SPI mode 3 to use TMC5130
	LL_SPI_SetClockPhase(SPI2, LL_SPI_PHASE_2EDGE);
	LL_SPI_SetClockPolarity(SPI2, LL_SPI_POLARITY_HIGH);

	// Request a reading on address
	GPIOB->BSRR |= LL_GPIO_PIN_12 << 16; //Set CS low
	this->status = this->spiHandle->transmit8BitData(address);
	this->spiHandle->transmit8BitData(0x00);
	this->spiHandle->transmit8BitData(0x00);
	this->spiHandle->transmit8BitData(0x00);
	this->spiHandle->transmit8BitData(0x00);
	GPIOB->BSRR |= LL_GPIO_PIN_12; //Set cs pin high

	// Read the actual value on second request
	int32_t value = 0;
	delayMicroseconds(1);
	GPIOB->BSRR |= LL_GPIO_PIN_12 << 16; //Set CS low
	this->status = this->spiHandle->transmit8BitData(address);
	value |= this->spiHandle->transmit8BitData(0x00);
	value <<= 8;
	value |= this->spiHandle->transmit8BitData(0x00);
	value <<= 8;
	value |= this->spiHandle->transmit8BitData(0x00);
	value <<= 8;
	value |= this->spiHandle->transmit8BitData(0x00);
	GPIOB->BSRR |= LL_GPIO_PIN_12; //Set cs pin high

	this->lastReadValue = value;

	return value;
}