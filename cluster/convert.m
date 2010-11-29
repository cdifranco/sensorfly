function result = convert(real_location_x, real_location_y, base_number,base)
load ('get_reading_data');
dist = ((real_location_x-base(:,1)).^2 + (real_location_y-base(:,2)).^2).^.5;
result = get_reading_fit_noise(dist,3,0.3,sigMeanDist);  
