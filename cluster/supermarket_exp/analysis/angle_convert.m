function angle = angle_convert(direction)
if  direction == 2 
    angle = mod(315 + unidrnd(90),360);
elseif direction == 1
    angle = 45 + unidrnd(90);
elseif direction == 4
    angle = 135 + unidrnd(90);
else
    angle = 225 + unidrnd(90);
end