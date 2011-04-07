% used for generate data set, used in generate cluster
function [next_point_index direction] = get_next_point(current_point_index, sigxy)
old_x = sigxy.x(current_point_index);
old_y = sigxy.y(current_point_index);
isvalid_point = 0;
while ~isvalid_point
    step_len = (3 + unidrnd(3)); %step length is from 30 cm to 60 cm
    angle = (rand*2-1)*pi;
    new_x = round(old_x/10 + step_len * cos(angle))*10;
    new_y = round(old_y/10 + step_len * sin(angle))*10;
    if isempty(find(sigxy.x == new_x & sigxy.y == new_y, 1)) 
        %fprintf('invalid points\n');
        continue;
    else
        isvalid_point = 1;
        next_point_index = find(sigxy.x == new_x & sigxy.y == new_y, 1);
    end
end
direction = direction_convert(angle);
