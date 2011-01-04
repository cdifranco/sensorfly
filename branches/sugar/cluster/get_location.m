function location = get_location()

location = [];

while length(location)~=2
    r1 = input('Please enter the real location(x)');
    r2 = input('Please enter the real location(y)');
    location = [location r1 r2]
    clear r1;
    clear r2;
end