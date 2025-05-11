void render_ship();

void handle_input();

void update();

float wrap(float given, float max);

void apply_friction(float *v, float amount);

float clamp(float val, float min, float max);
