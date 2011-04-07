% convert direction information into angle
function angle = angle_convert(direction)
if  direction == 2 
    angle = pi/2+(rand*pi/2-pi/4);
elseif direction == 1
    angle = 0+(rand*pi/2-pi/4);
elseif direction == 4
    angle = -pi/2+(rand*pi/2-pi/4);
else
    angle = pi+(rand*pi/2-pi/4);
    if angle > pi
        angle = angle - 2*pi;
    end
end