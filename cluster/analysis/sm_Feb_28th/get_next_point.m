function [next_point_x next_point_y] = get_next_point(current_point, std_signature)
next_point_x = -1;
next_point_y = -1;
if current_point(2) < 80
    next_point_y = current_point(2) + 2;
    next_point_x = current_point(1);
    while isempty(find(std_signature(:,1) == next_point_x & std_signature(:,2) == next_point_y,1)) && next_point_y < 80
        next_point_y = next_point_y + 2;
    end  
    if isempty(find(std_signature(:,1) == next_point_x & std_signature(:,2) == next_point_y,1))
        next_point_y = 0;
        if ~isempty(find(std_signature(:,1) == current_point(1) + 1, 1))
            next_point_x = current_point(1) + 1;
        else next_point_x = current_point(1) + 2;
        end
    end
else
    next_point_y = 0;
    if ~isempty(find(std_signature(:,1) == current_point(1) + 1, 1))
        next_point_x = current_point(1) + 1;
    else next_point_x = current_point(1) + 2;
    end
end