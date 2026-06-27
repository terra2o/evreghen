# to define the target
# add_library(engine STATIC src/core.cpp)
# we can get away with static linking for now

# for headers that SHOULDN'T be exposed to other subsystems:
# target_include_directories(PRIVATE engine/path/to/header)
# for headers that SHOULD be exposed to other subsystems:
# target_include_directories(PUBLIC engine/path/to/header)

