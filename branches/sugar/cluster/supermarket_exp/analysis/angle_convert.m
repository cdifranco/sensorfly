function angle = angle_convert(direction)
if  direction == 2 
    angle = mod(3150 + unidrnd(900),3600);
elseif direction == 1
    angle = 450 + unidrnd(900);
elseif direction == 4
    angle = 1350 + unidrnd(900);
else
    angle = 2250 + unidrnd(900);
end