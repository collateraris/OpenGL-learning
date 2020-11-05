#version 450 core

in vec3 vPassDir;
uniform vec3 uCamPos;

layout(binding = 2, std430) buffer nodePool_buffer
{
    int aNodePool[];
};

layout(binding = 3, std430) buffer nodeColor_buffer
{
    vec4 aNodeColor[];
};

#define is_leaf (node == -1)

uniform int uMaxLevel = 10; //only for level debugging!
uniform vec3 uBmin;
uniform vec3 uBmax;

void handleLeaf(in int nodeID, inout vec4 color, inout bool hit);
float compMax(vec3 v);
float compMin(vec3 v);
bool rayBoxIntersect( vec3 rpos, vec3 rdir, vec3 vmin, vec3 vmax, out float tMin, out float tMax);
bool rayInvBoxIntersect( vec3 rpos, vec3 rdir, vec3 vmin, vec3 vmax, out float tMin, out float tMax);
vec4 trace(in vec3 origin, in vec3 dir, in float tMin, in float tMax);

void main()
{
    ivec2 pixelPos = ivec2(gl_FragCoord.xy);
    vec3 dir = normalize(vPassDir);
    gl_FragColor = vec4(vec3(0.7f, 0.9f, 1.0f) + dir.y * 0.618f, 1.0f);

    vec4 r = trace(uCamPos, dir, 0, 1000);
    if (r.w >= 0)
    {
        gl_FragColor = r;
    }
}

void handleLeaf(in int nodeID, inout vec4 color, inout bool hit)
{
    if(aNodeColor[nodeID].w > 0.0f)
    {
        hit = true;
        color = aNodeColor[nodeID];
    }
}

float compMax(vec3 v)
{
    return max(v.x,max(v.y,v.z));
}

float compMin(vec3 v)
{
    return min(v.x,min(v.y,v.z));
}

bool rayBoxIntersect( vec3 rpos, vec3 rdir, vec3 vmin, vec3 vmax, out float tMin, out float tMax)
{
   vec3 t1 = (vmin - rpos)/rdir;
   vec3 t2 = (vmax - rpos)/rdir;
   tMin = compMax(min(t1,t2));
   tMax = compMin(max(t1,t2));
   return tMin < tMax;
}

bool rayInvBoxIntersect( vec3 rpos, vec3 rdir, vec3 vmin, vec3 vmax, out float tMin, out float tMax)
{
   vec3 t1 = (vmin - rpos)*rdir;
   vec3 t2 = (vmax - rpos)*rdir;
   tMin = compMax(min(t1,t2));
   tMax = compMin(max(t1,t2));
   return tMin < tMax;
}

vec4 trace(in vec3 origin, in vec3 dir, in float tMin, in float tMax)
{
    if (!rayBoxIntersect(origin, dir, uBmin, uBmax, tMin, tMax) || tMax < 0.f)
    {
        return vec4(-1);
    }

    tMin = max(tMin, 0.f);

    vec3 rootSize = uBmax - uBmin;
    ivec3 nodePos = ivec3(0, 0, 0);
    ivec3 cIndex = ivec3(greaterThanEqual((origin + tMin * dir - uBmin), vec3(0.5 * rootSize)));

    int nodeID = 0;
    int stackPointer = 0;
    int voxelSizeFactor = 1;

    bool hit = false;

    dir = max(abs(dir), vec3(1e-7)) * sign(dir);
    vec3 invDir = 1.f / dir;
    vec3 step = vec3(dir.x < 0.f ? -1.f : 1.f, dir.y < 0.f ? -1.f : 1.f, dir.z < 0.f ? -1.f : 1.f);
    int node;

    int stack[10];
    stack[0] = 0;

    while(stackPointer >= 0)
    {
        node = aNodePool[nodeID];
        //check if current node is not leaf node
        if (node > 0 && cIndex.x != -1)
        {
            stackPointer++;
            nodeID = node + cIndex.x + (cIndex.y << 1) + (cIndex.z << 2);
            stack[stackPointer] = nodeID;
            nodePos = (nodePos << 1) + cIndex;
            voxelSizeFactor <<= 1;

            if (stackPointer >= uMaxLevel)
            {
                vec4 tr;
                handleLeaf(nodeID, tr, hit);

                if (hit)
                {
                    return tr;
                }
            }

            vec3 voxelSize = rootSize / voxelSizeFactor;
            vec3 minPos = vec3(nodePos) * voxelSize + uBmin;
            vec3 maxPos = minPos + voxelSize;
            rayInvBoxIntersect(origin, invDir, minPos, maxPos, tMin, tMax);
            cIndex = ivec3(greaterThanEqual((origin + max(0, tMin) * dir - minPos), vec3(0.5 * voxelSize)));

        }
        else
        {
            if (node == -1)
            {
                vec4 tr;
                handleLeaf(nodeID, tr, hit);

                if (hit)
                {
                    return tr;
                }
            }

            //this node is not needed on the stack anymore
            stackPointer--;
            nodeID = stack[stackPointer];
            cIndex = nodePos & 0x1; // %2
            nodePos >>= 1;
            voxelSizeFactor >>= 1;

            //calculate next childIndex.
            vec3 voxelSize = rootSize / voxelSizeFactor;
            vec3 pos = (vec3(nodePos) + vec3(cIndex) * 0.5f + 0.25f + step * 0.25f) * voxelSize + uBmin;
            vec3 tStep = (pos - origin) * invDir;
            float tMin2 = compMin(tStep);
            int tStepMinIndex = int(tStep.y == tMin2) + (int(tStep.z == tMin2) << 1);
            int cIndexNew = cIndex[tStepMinIndex] += int(step[tStepMinIndex]);
            if (cIndexNew < 0 || cIndexNew > 1)
            {
                cIndex.x = -1;
            }
        }
    }
    return vec4(-1);
}
