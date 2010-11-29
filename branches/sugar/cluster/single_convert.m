function result = single_convert(real_location_x, real_location_y, base_number,base)
x = [1,1.5,3];
y = [4:1.0:9];
for j = 1 : base_number
    result(j) = interp2(x,y,base{j},real_location_x,real_location_y,'linear');
end
