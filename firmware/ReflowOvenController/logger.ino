
void logger_log (int timerSeconds, double setpoint, double input, double output, const char* state)
{
	// Send temperature and time stamp to serial
	Serial.print(timerSeconds);
	Serial.print("\t");
	Serial.print(setpoint);
	Serial.print("\t");
	Serial.print(input);
	Serial.print("\t");
	Serial.print(output);
	Serial.print("\t");
	Serial.println(state);
}