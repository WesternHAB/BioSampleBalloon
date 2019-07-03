sensors = ["bme", "position", "gps"]
MinimalSampleRegions = [0, 10, 20, 30] # km
IdealSampleRegions = [0, 5, 10, 15, 20, 25, 30] # km
altitudeRange = 1 # km
recordingTime = 30 # s

Loop: if most recent FlightLog entry includes data from all sensors:
		Turn on Green indicator light (healthy payload)
	else
		Turn on Red light (unhealthy payload)

	# use the GPS to determine when we open the sample chambers
	if GPS entry is "good" and balloon is in ascent phase:
		get altitude

		sampleRegion = getRegion(altitude, altitudeRange) # index from 0-6

		triggerLogging(recordingTime) # start camera to record for preset amount of time
		openChamber(chamber[sampleRegion]) # opens the corresponding chamber
		closeChambers(chamber[:sampleRegion], chamber[sampleRegion+1:]) # close all other chambers
	else if GPS entry is "good" and balloon is in descent phase:
		close all SampleChambers
	else if GPS entry is "bad" over the last x minutes:
		close all SampleChambers

	# robust sample chamber logging for confirming integrity of data.
	def triggerLogging(recordingTime):
		start camera recording for time specified by `recordingTime`
		log position reading of all actuators
		log open/closing action of chamber, including timestamp
		log filename of associated camera recording
Repeat Loop
