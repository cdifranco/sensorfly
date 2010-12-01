basic_time = [];
basic_accuracy = [];
kmeans_time = [];
kmeans_accuracy = [];
random_time = [];
random_accuracy = [];

for i = 1:size(time,2)
    if mod(i,3)==1 
        basic_time = [basic_time time(i)];
        basic_accuracy = [basic_accuracy accuracy(i)];
    elseif mod(i,3)==2 
        kmeans_time = [kmeans_time time(i)];
        kmeans_accuracy = [kmeans_accuracy accuracy(i)];
    else 
        random_time = [random_time time(i)];
        random_accuracy = [random_accuracy accuracy(i)];
    end
end
%{
plot(10:9+size(time,2)/3,basic_time,'r',10:9+size(time,2)/3,kmeans_time,'b',10:9+size(time,2)/3,random_time,'g');
figure;
plot(10:9+size(time,2)/3,basic_accuracy,'r',10:9+size(time,2)/3,kmeans_accuracy,'b',10:9+size(time,2)/3,random_accuracy,'g');
%}
plot(10:9+size(time,2)/3,basic_time,'r',10:9+size(time,2)/3,kmeans_time,'b',10:9+size(time,2)/3,random_time,'g');
figure;
plot(10:9+size(time,2)/3,basic_accuracy,'r',10:9+size(time,2)/3,kmeans_accuracy,'b',10:9+size(time,2)/3,random_accuracy,'g');