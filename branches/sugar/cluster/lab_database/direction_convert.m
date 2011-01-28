function dir = direction_convert(real_dir)
if  real_dir <= 900
    dir = 4;
elseif real_dir <= 1800
    dir = 3;
elseif real_dir <= 2700
    dir = 2;
else
    dir = 1;
end