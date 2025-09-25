# Algorithms

## Obstacle Avoidance
- Uses HC-SR04 sensor.
- If distance < 20 cm → stop + turn left or right.
- Decision made using color comparison (red/green) or default turn.

## Lap Counting
- TCS3200 reads ground colors.
- Sequence BLUE → ORANGE = one corner.
- After 4 corners = 1 lap.
- After 3 laps = stop.

## State Machine
- WAIT_FIRST → waiting for first color.
- WAIT_SECOND → expecting second color within 1200 ms.
- Cooldown of 1500 ms to avoid double counting.
