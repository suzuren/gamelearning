local timerUtility = require "timer"

local TIMER  = {
	TICK_STEP = 100,
	TICKSPAN_ANDROID_INOUT = 5,
	TICKSPAN_DISTRIBUTE_ANDROID = 8,
	TICKSPAN_LOAD_ANDROID_USER = 3600,
	TICKSPAN_TABLE_OFFLINE_WAIT = 60,
}

local function printf_hello() -- pre 5 s
	print("hello world")
end

timerUtility.start(TIMER.TICK_STEP)
timerUtility.setInterval(printf_hello, 5)

