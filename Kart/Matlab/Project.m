% Project.m
% Edge Based Text region extraction in images
% By Karthik Deepak Natraj 

warning('off','Images:im2bw:binaryInput');
% Read the input image

I = imread('I.png');
Out=~im2bw(I);
%Preallocate arrays to improve speed
Pyramid=cell(1,4);Conv=cell(1,4,1,4);Conv2=cell(1,4,1,4);comb=cell(1,4,1,4);
% The direction filters
%0 degree
K{1} = [-1 -1 -1 ; 2 2 2;-1 -1 -1]; 
%45 degree
K{2} = [-1 -1 2 ; -1 2 -1 ;2 -1 -1];
%90 degree
K{3} = [-1 2 -1 ; -1 2 -1 ; -1 2 -1];
%135 degree
K{4} = [2 -1 -1 ; -1 2 -1 ; -1 -1 2];
% Creating Gaussian Pyramid using 
gfil = fspecial('gaussian');   
%Convolve with direction filters and Resize images to original image size'
I1=I;
for i = 1:4
    %convolve with gaussian filter and down-sample by 0.5
    Pyramid{i} = I1;
    I1 = imresize(I1,0.5);
    I1 = imfilter(I1,gfil,'conv');
    for j = 1:4
        Conv{i,j} = imfilter(Pyramid{i},K{j},'conv');
    end
end
for i = 1:4
    for j = 1:4
        Conv2{i,j} = imresize(Conv{i,j},[size(I,1) size(I,2)]);
    end
end

%Find the sum all directional filter responses
for i= 1:4
    comb{i}= im2bw((Conv2{1,i}+Conv2{2,i}+Conv2{3,i}+Conv2{4,i}));
end
Total = imadd((comb{1,1}+comb{1,3}),(comb{1,2}+comb{1,4}));

%Dilation and Closing with SE 
Edge = im2bw(comb{1,3});
I_Dil = imdilate(Edge,strel('line',3,0));
I_Clos = imclose(I_Dil,strel('line',(round(size(Edge,1)/25)),90));

% Find the weak edges and combine them with the previous edge
EdgeWeak = I_Clos - I_Dil;
Edge_horizontal = Edge + EdgeWeak;

%Perform thinning and remove long edges
Thinned = bwmorph(Edge_horizontal,'thin',1/0);
[L,N] = bwlabel(Thinned,4);
S = regionprops(L,'all');

for i=1:length(S)
    if S(i).MajorAxisLength > (size(I,1)/5)
        j = S(i).PixelList(:,1);
        k = S(i).PixelList(:,2);
        Thinned(k,j)=0;
    end
end
Refined = immultiply(imdilate(Thinned,strel('line',5,90)),Total);
ref = imdilate(Refined,strel('square',4));

%Feature Map 
T=(im2bw(comb{1,1})&im2bw(comb{1,2}))+(im2bw(comb{1,3})&im2bw(comb{1,4}));
Fmap = (ref&T); 
NewImage = imdilate(Fmap,strel('disk',6));

%Filter the image using area based filtering
[Lab,Num] = bwlabel(NewImage,4);
Regions = regionprops(Lab,'all');
MaxArea=0;
for i=1:length(Regions)
    Area = Regions(i).Area;
    if(MaxArea < Area)
        MaxArea = Area;
    end
end
MaxArea=MaxArea/20;
for i=1:length(Regions)
    A = Regions(i).Area;
    if(A < MaxArea)
        NewImage = bwareaopen(NewImage,A);
    end
end
for i=1:length(Regions)
    if (Regions(i).MajorAxisLength / Regions(i).MinorAxisLength)>6
        j = Regions(i).PixelList(:,1);
        k = Regions(i).PixelList(:,2);
        NewImage(k,j)=0;
    end    
end
%Final Output image
Out = immultiply(Out,NewImage);
figure,imshow(Out),title('Output Image');