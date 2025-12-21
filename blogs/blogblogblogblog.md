`
From now on this will be the blog space since I do not want to have limited png. In my previous blogs I could not write to my will so here it is.
Behold, pictures.

Also, initially I was so scared of adding textures because it seemed like just the coding part took forever. Turns out debugging wasn't as hard.


---
# Texture Maps

To add texture maps into my raytracer, I first created a dedicated folder than shoved every code into a single file (as one does).

I created the following Texture class to work with as a base:
```c++
class Texture
{
public:
    virtual ~Texture() = default;
    uint32_t _id;
    DecalMode decalMode;
    Texture(uint32_t i, DecalMode d) : _id(i), decalMode(d){}
    virtual Color TextureColor(const Vertex& vert, Texel& tex) = 0;
    virtual TextureType getTextureType() = 0;
};
```

TextureColor gets the color of the texture at a certain point. Since I use texel for images and vertex for others I pass both. 
Which is not efficient if texel is not being used because it is computed either way.

## Using Texture
To be able to use the textures, I added four texture pointers to my abstract object class.
One for each: 
- diffuse: replace_kd
- specular: replace_ks
- normal: replace_normal and bump_normal
- all: replace_all
This also means I am not accumulating textures in the case of an object having several texture of the same type. 
It is possible to implement that, but not needed as I believe this makes more sense.

I also added several functions to the object class:
- GetColourAt: The colour computation is now completely handled at the object class. This function is called after shadow testing is done. If there is a `replace_all` texture, then not even shadow testing is being done and directly texturecolor is called.
```c++
Color Object::GetColourAt(Color ambientLight, Color I_R_2, real cos_theta, const Vec3r& normal, const Ray& ray, Ray& shadow_ray, real time, int triID) const
{
    Texel tex = getTexel(shadow_ray.pos,time, triID);
    Color diffuse = diffuseTerm(I_R_2, cos_theta, shadow_ray.pos, tex);
    Color specular = specularTerm(normal, ray, I_R_2, shadow_ray, shadow_ray.pos, tex);
    Color ambient = ambientLight * material.AmbientReflectance;
    return  diffuse + specular + ambient;
}
```
- (virtual) getTexel: Returns texel coordinates of a vertex according to the time (if there is motion blur).
- (virtual) getBitan: this function computes the T and B vectors. I struggled quite a bit to make this correct for the triangles (see debugging).
- computeBitan: For triangles and planes I initially compute their tangents since theirs' don't change. For this purpose I have this function to pre-compute.
- getTexturedNormal: this is called in every getNormal function right before returning. Only manipulates the normal if there is a normal texture. Will be talked more about in displacement mapping section.


## ImageTexture

Coding the image has several layers:

### Image class
```c++
class Image
{
public:
    uint32_t _id;
    int channels_in_file, width, height;
    std::vector<std::vector<Color>> colorData;
    Image(uint32_t id, std::string filename);
    ~Image();
};
```

Now you might realize that I am holding colordata as a color vector instead of a char array. That is because while loading the texture I simply fill that array for the ease of coding.

```c++
uint32_t curr_idx = 0;
colorData.resize(height, std::vector<Color>(width));
for (int y = 0; y < height; y++){
    for (int x = 0; x < width; x++){
        colorData[y][x] = Color(data[curr_idx], data[curr_idx+1], data[curr_idx+2]);
        curr_idx += 3;
    }
}
```

This is open to improvement since currently I only store the RGB and if the channel numbers are different and I need that  the program won't work as accepted. But no issue for now, let's continue with loading the image.

### Loading the texture image
For this purpose I used the stb library and the function `stbi_load`. It is important to add the header to the folder and include it as follows:
```c++
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
```
Works flawlessly. 
Though I did have an issue when I tried to store data pointer in the Image class. 
There would be a segfault everytime at the exact place where the Image class constructor would return. 
I did not solve this issue as I asked to myself "Do I even need the pointer?". Which I didn't, so I removed it. 
I assume I need to deep copy it for it to work. Which is kind of what I already do with my colorData vector.




### ImageTexture class
Finally, the class itself with all its glory.

```c++

class ImageTexture : public Texture
{
public:
    ImageTexture(uint32_t id, DecalMode d, Image *image, Interpolation interp);
    Color TextureColor(const Vertex& vert, Texel& tex) override;
    TextureType getTextureType() override;

private:
    Color ImageColor(int x, int y);
    Image *image;
    
    // interpolation
    std::function<Color(Texel)> interpolate;
    Color nearest(Texel texel);
    Color bilinear(Texel texel);
    Color trilinear(Texel texel);
};
```
There are a few things to unpack here. So let's go step by step.

**TextureColor:**
This is the virtual function from the Texture class. It is very straightforward for image textures since the actual work is being done by the interpolation functions. 
```c++
interpolate(tex)/ 255.0;
```
It simply does and returns this. I divide it by 255 because for most cases, we need the result to be in range 0 and 1. 
The ranges do change for normals and replace all decal types so do multiply/divide once more in them.
Could be more efficient. Not for now.

**ImageColor:**
This function gets an x and y coordinate on the image and returns the colour of it. X and y are clamped to prevent out of bounds access.

**Interpolation:** 
This is the big part.

I hold the interpolation as an std::function which is set to be one of the three interpolation functions within the class.
I then call the interpolation function whenever I want and skip the if-elses or switches. Looks cleaner.

To be honest, I did have a hard time getting the interpolations exact. Mine would always be off a bit. Their final exact versions are below:
```c++
Color ImageTexture::nearest(Texel texel)
{
    Texel xy = {texel.u * (image->width), texel.v * (image->height)};
    return ImageColor(std::round(xy.u - 0.5), std::round(xy.v - 0.5));
}
```
As you might realize, I subtract 0.5 from both x and y. That is because we need to round it to the nearest pixel's **center**, not its corner vertex.
Now please don't think that I thought of it from the get-go. No. No I sadly did not.


```c++
Color ImageTexture::bilinear(Texel texel)
{
    Texel xy = {texel.u * (image->width), texel.v * (image->height)};
    Texel pq = {std::floor(xy.u), std::floor(xy.v)};
    Texel delta = {xy.u - pq.u, xy.v - pq.v};
    Color result = ImageColor(pq.u, pq.v) * (1 - delta.u) * (1 - delta.v)
        + ImageColor(pq.u + 1, pq.v) * (delta.u) * (1 - delta.v)
        + ImageColor(pq.u, pq.v + 1) * (1 - delta.u) * (delta.v)
        + ImageColor(pq.u + 1, pq.v + 1) * (delta.u) * (delta.v);
    return result;
}

```
Funnily enough, bilinear interpolation was more straightforward for me. Of course, that does not mean I had no issues. More will be talked about in the debugging section.

// TODO mipmapping?



### Debugging
<p align="center">
  <img src="img.png" width="400">
  <figcaption></figcaption>
</p>

Marvelous. But we cannot have that. So let's fix it. The problem is how I convert the texels to the image texels. Spoiler: I don't. 
That is why all I get the very first pixel. This issue mainly is about the interpolation functions. Let's check them.

**nearest:**

**bilinear:**


## Checkerboard
This is quite cute compared to the image texture. Holds a black colour, a white colour, scale and offset. Has a `IsOnWhite` function that is called by texture colour function. 
Look at the beauty, the simplicity. I of course managed to make a mistake.

```c++
Color CheckerTexture::TextureColor(const Vertex& vert, Texel& tex)
{
    return IsOnWhite(vert) ? whiteColor : blackColor;
}

bool CheckerTexture::IsOnWhite(real i)
{
    real place = (i + offset) * scale;
    return ((int) std::floor(place) & 1) == 1;
}

bool CheckerTexture::IsOnWhite(Vertex vert)
{
    bool xorXY = IsOnWhite(vert.x) != IsOnWhite(vert.y);
    return xorXY == IsOnWhite(vert.z);
}
```

Initially the isOnWhite(real i) would perform the following:
```c++
    real place = (i + offset) * scale;
    return ((int) std::floor(place) % 2) == 1;
```

Which resulted in this in VeachAjar:
![img_21.png](img_21.png)

To be honest I fixed it by trial and error and didn't really get what was wrong in the initial one at first. Apparently it is about the negative values. When I take the modulo, the result stays negative if the initial value is negative. This of course leads to more "false"s when I check for `== 1`.

Also I would like to note that I am using the global vertex here instead of localizing it or using the texel.

I had issue with VeachAjar scene, where my `replace_kd` textures where not bright enough. I did not have this issue anywhere else and when I multiplied kd by 8.50 I got the intended results. 

I just realized there is a "normalizer" aspect. My bad.

![img_22.png](img_22.png)

Well that didn't fix anything. I swear my checkerboard works it is just too dark.
Oh I am dividing by an extra 255 and multiplying with normalizer instead of dividing. It is fixed now but instead of showing the final picture I will brag about my sampling from previous homework which I was not able to show on the previous blog. Mine are the ones on the left.


Side note: I still did not fix my dielectrics.

![img_26.png](img_26.png)
![img_27.png](img_27.png)




## Perlin Noise
Here we go. I implemented the perlin noise class from !!!!!TODO!!!!!!!!!!.

Let's see what is the initial result I got.

Now this, this requires attention. I don't know if you can tell.

<p align="center">
  <img src="img_2.png" width="400">
  <figcaption></figcaption>
</p>

Probably if you squint. Anyways let's fix it. I already implemented perlin noise on OpenGL and saw a similar issue. Not that I remember how to fix it.
Good thing is we know that our checkerboard class works flawlessly. So the problem is simply with perlinNoise. 

Turns out I forgot to initialize the perlin noise function which randomizes the P array.
 
Also, I forgot to call my convertNoise function

<p align="center">
  <img src="img_5.png" width="400">
  <figcaption></figcaption>
</p>

Ah yes, a lot more familiar.

I then changed the fade function to be in the website rather than the slides and voila

<p align="center">
  <img src="img_6.png" width="400">
  <figcaption></figcaption>
</p>

Now, you can see that there is no difference between absolute and linear. Let me present you the reason:
```c++
if (funcname == "absval)")return Convert::Abs;
if (funcname == "linear)")return Convert::Linear;
return Convert::Linear;
```
Truly a work of art in terms of mistakes.

Bonus: During debugging of VeachAjar scene I tried to localize global instance vertices which lead to this 
(The perlin noise looks "flat"):
![img_20.png](img_20.png)



I also wanted to try something and changed my absolute value function to return the negative of its result. I am mentioning this because I believe the result looked really cool.
![img_28.png](img_28.png)

I was debugging because my dragon's colours were way too vibrant than the reference.
![img_29.png](img_29.png)

I sadly could not find the reason to this.
## Colour Mapping

### Diffuse
### Specular
### Background
I would initially get a black background because I forgot to multiply by 255.

Below is a version with perlin noise that I tried just to see

<p align="center">
  <img src="img_10.png" width="400">
  <figcaption></figcaption>
</p>
<p align="center">
  <img src="img_8.png" width="400">
  <figcaption></figcaption>
</p>

Then I tried checkerboard, which looked like this. 
It was because I was using ``viewing_ray.pos + viewing_ray.dir``. I then changed it to use the texel coordinates with `z=0`.

<p align="center">
  <img src="img_9.png" width="400">
  <figcaption></figcaption>
</p>

### Replace all

## Displacement Mapping

We implement two types of displacement mapping, normal replacement and bump mapping. They do have common parts so let's start with that. 
Mainly, we will be talking about the getTexturedNormal function.

### Normal Replacement

- Step 1: get the texture colour
- Step 2: Turn the colour into a normal vector
```c++
Vec3r locNormal(textureColor.r * 2- 1.0 , textureColor.g * 2 - 1.0 , textureColor.b *2 - 1.0 );
locNormal = locNormal.normalize();
```

- Step 3: Get the tangent and bitangent (i.e. the orthonormal basis we will be using)

- Step 4: Apply the orthonormal basis to the texture normal we got at step 2
```c++
for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
        NewN[(Axes) i] += locNormal[(Axes) j] * onb[j][(Axes) i];
```

I overloaded the [] operator so 0 -> x, 1 -> y, 2 -> z. This enables me to use loops (and use `=` instead of `+=` to add to the difficulty).

**Debugging:**
**getBitan:** (in Sphere) Instead of using the local x y and z vertices, I directly used the global vertex coordinates. Which, of course, did not work.
I now do the following `Vertex v_local = v_global - center`.

**ComputeBitan:** Oh boy. This was a long one. Turns out I was multiplying my matrices in the wrong row-column order.
Moreover, I had this consistent mistake where I would use `=` instead of `+=` which got rid of all the accumulation in my loops. Also I did not normalize T and B at first.

If there is a mistake to be made, you can be sure that I am making it.

I also would like to note that I had **less** issues in this homework than in the previous ones. I just did not have time to write them to my blog previously. But now that I am quite content with the code I already have. It is way easier to debug and make additions.


I do have slight differences yet to be solved TODO ÇÖZDÜN MÜ

### Bump Mapping

<p align="center">
  <img src="img_18.png" width="400">
  <figcaption></figcaption>
</p>

**Debugging Image Textures**
<p align="center">
  <img src="img_1.png" width="400">
  <figcaption></figcaption>
</p>

That will not do. 
I was trying to do image bump mapping according to the vertices, not texels so I rewrote the image part.
<p align="center">
  <img src="img_12.png" width="400">
  <figcaption></figcaption>
</p>

No, this will not as well.


I tried so long to make bump images look good. But I managed in the end. The main problem was that I was normalizing B and T vectors. It got fixed when I stopped normalizing them specifically in image bump maps.

![img_19.png](img_19.png)

Truly an artistic rendition. You can't deny. After fixing bump mapping in other scenes this scene also mostly got fixed. But somehow I ruined the background?

![img_30.png](img_30.png)
Mine is on the left.

**Debugging Noise Textures**

<p align="center">
  <img src="img_16.png" width="400">
  <figcaption></figcaption>
</p>

The upper picture was the result of me forgetting to multiply with the bump factor.

<p align="center">
  <img src="img_17.png" width="400">
  <figcaption></figcaption>
</p>

The main mistake I was making was normalizing `g_perp`. Below is the final correct version:
```c++
Vec3r g_parall  = n *dot_product(dh,n);
Vec3r g_perp    = dh - g_parall;
NewN = n - g_perp* NormalTexture->bumpFactor;
```
Because normalizing is a reflex at this point. 

![img_31.png](img_31.png)
Oh god I thought I had fixed it.

So the problem was the following:
1. It was a plane, not a triangle
2. This meant the T and B computed were from ONB computations (normalized) and were really big
3. This resulted in `g_perp = { 0.3, 0.0, 0.98}` when `n={0,1,0}`
4. This meant that my g would basically punch my normal to the side.

How to fix this then? I also realized my computation was really open to such flaws so I rewrote that part (for the 3289th time).

I was computing delta x by moving the point along t in every direction, delta y with B and delta z with the normal. What was I thinking, I do not know.
I then computed delta h with T and B separately and combined it as such:

```c++
g_perp = onb[0] * dhT + onb[1]*dhB;
```

Which mimics texel coordinates I believe.

This fixed it :)

![img_32.png](img_32.png)


# Mipmapping
Welcome to my debugging gallery. Below you can find my paintings.
![img_33.png](img_33.png)
Highway to hell.

![img_34.png](img_34.png)
mipmapping fractals

# Speedup
My textures were coded non-optimal and I am sick of waiting. Let's fix it because I will not be waiting for killeroo.



# Final


I had time this time, does it show?`