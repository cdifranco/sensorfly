function dir = direction_convert(real_dir)
if  real_dir <= 900
    dir = 1;
elseif real_dir <= 1800
    dir = 2;
elseif real_dir <= 2700
    dir = 3;
else
    dir = 4;
end