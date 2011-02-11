function [next_point_x next_point_y] = get_next_point(current_point)
next_point_x = -1;
next_point_y = -1;
if current_point(2) < 46
    next_point_y = current_point(2) + 1;
    next_point_x = current_point(1);
else
    next_point_y = 0;
    if current_point(1) < 7
        next_point_x = current_point(1) + 1;
    end
end