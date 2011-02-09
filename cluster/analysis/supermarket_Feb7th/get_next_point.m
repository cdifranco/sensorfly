function [next_point_x next_point_y] = get_next_point(current_point)
next_point_x = -1;
next_point_y = -1;
if current_point(2) < 80
    next_point_y = current_point(2) + 10;
    next_point_x = current_point(1);
else
    next_point_y = 0;
    if current_point(1) < 27
        next_point_x = current_point(1) + 10;
    end
end