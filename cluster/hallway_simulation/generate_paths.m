RandStream.setDefaultStream(RandStream('mt19937ar','seed',sum(100*clock)));
path = zeros(10000,4);    
for i = 1 : 10000
    startX = 3*rand;
    if startX <= 1
        startY = 4*rand;
    else
        startY = 9*rand;
    end
    destX = 3*rand;
    if destX <= 1
        destY = 4*rand;
    else
        destY = 9*rand;
    end
    path(i,:) = [startX startY destX destY];
end
    