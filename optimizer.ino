void optimizeSpeed() {
  if (allowSpeedOptimizer == 1) {
    float gy = getGy() * 10;
    if (gy < optimalMinG) {
      float diffToOptimal = optimalMinG - gy;
      if (diffToOptimal > 1) {
        setSpeed(maxSpeedUp);
      } else if (diffToOptimal > 0.5) {
        setSpeed(halfSpeedUp);
      } else {
        setSpeed(littleSpeedUp);
      }
    } else {
      setSpeed(getPresetSpeed());
    }
  }
}