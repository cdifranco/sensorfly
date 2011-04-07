function dir = direction_convert(angle)
% Convert angle (radian, in[-pi, pi)) to direction, right is 0, up is 1, left is 2 and down is 3
if -3*pi/4 <= angle && angle < -pi/4
    dir = 4;
elseif -pi/4 <= angle && angle < pi/4
    dir = 1;
elseif pi/4 <= angle && angle < 3*pi/4
    dir = 2;
else
    dir = 3;
end