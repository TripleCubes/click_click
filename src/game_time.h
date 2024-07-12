#ifndef GAME_TIME_H
#define GAME_TIME_H

struct GameTime {
	float delta = 0;
	float frame_time = 0;
	float time_since_start = 0;
	int frame_passed = 0;
};

#endif
