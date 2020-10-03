class AQICalculator 
{
public:
	int get_aqi(uint16_t pm2_5_value, uint16_t pm10_0_value) 
	{
		int pm2_5_index = calculate_index_(pm2_5_value, pm2_5_calculation_grid_);
		int pm10_0_index = calculate_index_(pm10_0_value, pm10_0_calculation_grid_);

		// don't have spurious values
		if ((pm2_5_value > 1000) && (pm10_0_value > 1000)) return 0;
		if (pm2_5_value > 1000) return pm10_0_index;
		if (pm10_0_value > 1000) return pm2_5_index;
		return (pm2_5_index < pm10_0_index) ? pm10_0_index : pm2_5_index;
	}

	// EPA correction for purpleair PM2.5 measurements
	// see https://cfpub.epa.gov/si/si_public_record_report.cfm?Lab=CEMM&dirEntryId=349513
	// @param rawPM2p5	raw PM2.5 value from sensor
	// @param humidity	humidity percentage value, i.e. 0-100
	uint16_t GetCorrectedPM2p5(uint16_t rawPM2p5, float humidity)
	{
		return int(0.52 * (float)rawPM2p5 - 0.085 * humidity + 5.71);
	}

protected:
	static const int AMOUNT_OF_LEVELS = 6;

	int index_grid_[AMOUNT_OF_LEVELS][2] = {{  0,  51}, 
											{ 51, 100}, 
											{101, 150}, 
											{151, 200}, 
											{201, 300}, 
											{301, 500}};

	int pm2_5_calculation_grid_[AMOUNT_OF_LEVELS][2] = {{  0,  12}, 
														{ 13,  35}, 
														{ 36,  55}, 
														{ 56, 150}, 
														{151, 250}, 
														{251, 500}};

	int pm10_0_calculation_grid_[AMOUNT_OF_LEVELS][2] = {{  0,  54},
														 { 55, 154},
														 {155, 254},
														 {255, 354},
														 {355, 424},
														 {425, 604}};

	int calculate_index_(uint16_t value, int array[AMOUNT_OF_LEVELS][2]) 
	{
		int grid_index = get_grid_index_(value, array);
		float aqi_lo = index_grid_[grid_index][0];
		float aqi_hi = index_grid_[grid_index][1];
		float conc_lo = array[grid_index][0];
		float conc_hi = array[grid_index][1];

		return (int)(((aqi_hi - aqi_lo) / (conc_hi - conc_lo)) * ((float)value - conc_lo)) + aqi_lo;
	}

	int get_grid_index_(uint16_t value, int array[AMOUNT_OF_LEVELS][2]) 
	{
		for (int i = 0; i < AMOUNT_OF_LEVELS; i++) 
		{
			if ((value >= array[i][0]) && (value <= array[i][1])) 
			{
				return i;
			}
		}
		return AMOUNT_OF_LEVELS - 1;
	}
};
AQICalculator aqicalc;
