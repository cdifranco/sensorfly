function dir = direction_convert(real_dir, reference_dir)
relative_dir = mod(reference_dir - real_dir, 3600);
if  relative_dir <= 450 || relative_dir > 3150
    dir = 2;
elseif real_dir <= 1350
    dir = 1;
elseif real_dir <= 2250
    dir = 4;
else
    dir = 3;
end