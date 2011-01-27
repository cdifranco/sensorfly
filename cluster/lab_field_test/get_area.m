function area = get_area(area_number)
area = -1;
area_id = 1:area_number;
while isempty(find(ismember(area_id, area),1))
    area = input('Please enter the area id: ');
end