## 目录
- [数据算法](#章节二)
     - [双指针](#删除有序数组重复项)
     - [哈希表](#子章节二)
- [1/26删除有序数组重复项](#删除有序数组重复项)
- [章节二](#章节二)
- [章节三](#章节三)


## 删除有序数组重复项

给你一个有序数组 nums ，请你 原地 删除重复出现的元素，使得出现次数超过两次的元素只出现k次 ，返回删除后数组的新长度。

不要使用额外的数组空间，你必须在 原地 修改输入数组 并在使用 O(1) 额外空间的条件下完成。

**双指针**

```
class Solution {
public:
    int removeDuplicates(vector<int>& nums) {
        int p1=0;
        int p2=1;
        int cout=1;
        int n=nums.size();
        if (n <= k) {
            return n;
        }
        while(p2<n)
        {
            if(nums[p1]==nums[p2]&&cout<k)
            {
                nums[++p1]=nums[p2];
                cout++;
            }
            else if(nums[p1]!=nums[p2])
            {
                cout=1;
                nums[++p1]=nums[p2];
            }
            p2++;
        }
        return p1+1;
    }
};
```

优化思路：对于前 k 个数字，直接保留；与当前写入的位置前面的第 k 个元素进行比较，不相同则保留

```
class Solution {
public:
    int removeDuplicates(vector<int>& nums) {
return process(vector<int>& nums,k);
};
int process(vector<int>& nums,k){
int n=nums.size();
        if (n <= k) {
            return n;
        }
    int slow = k, fast = k;
while (fast < n) {
if (nums[fast] != nums[slow - k]) { //nums[slow - k] 是当前考虑的元素在新数组中的第一个可能的位置
            nums[slow] = nums[fast];
            slow++;
        }
        // 将 fast 向前移动一位，以检查下一个元素
        fast++;
    }
    return slow;
}
```

**哈希表**
