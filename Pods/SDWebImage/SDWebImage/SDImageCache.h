/*
 * This file is part of the SDWebImage package.
 * (c) Olivier Poitrey <rs@dailymotion.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#import <Foundation/Foundation.h>
#import "SDWebImageCompat.h"

// image的来源，是来自网络下载，还是内存缓存，disk缓存
typedef NS_ENUM(NSInteger, SDImageCacheType) {
    /**
     * The image wasn't available the SDWebImage caches, but was downloaded from the web.
     */
    SDImageCacheTypeNone,
    /**
     * The image was obtained from the disk cache.
     */
    SDImageCacheTypeDisk,
    /**
     * The image was obtained from the memory cache.
     */
    SDImageCacheTypeMemory
};

// 查询的block
typedef void(^SDWebImageQueryCompletedBlock)(UIImage *image, SDImageCacheType cacheType);

// 检查缓存的block
typedef void(^SDWebImageCheckCacheCompletionBlock)(BOOL isInCache);

// 计算文件的block，待定，暂时不知道具体的作用
typedef void(^SDWebImageCalculateSizeBlock)(NSUInteger fileCount, NSUInteger totalSize);


/**
 * SDImageCache maintains a memory cache and an optional disk cache. Disk cache write operations are performed
 * asynchronous so it doesn’t add unnecessary latency to the UI.
 * 维护一个内存缓存，disk缓存是可选的，而且是一部操作，不会影响主线程
 */
@interface SDImageCache : NSObject

/**
 * Decompressing images that are downloaded and cached can improve performance but can consume lot of memory.
 * Defaults to YES. Set this to NO if you are experiencing a crash due to excessive memory consumption.
 * 如果app 由于内存问题，可以把这个值设置为no，默认设为yes，暂时不知道这个解压时什么意思？
 */
@property (assign, nonatomic) BOOL shouldDecompressImages;

/**
 *  disable iCloud backup [defaults to YES]
 *  可以开启备份到iCloud，默认是关闭的
 */
@property (assign, nonatomic) BOOL shouldDisableiCloud;

/**
 * use memory cache [defaults to YES]
 * 缓存到内存中，默认是yes；
 */
@property (assign, nonatomic) BOOL shouldCacheImagesInMemory;

/**
 * The maximum "total cost" of the in-memory image cache. The cost function is the number of pixels held in memory.
 */
@property (assign, nonatomic) NSUInteger maxMemoryCost;

/**
 * The maximum number of objects the cache should hold.
 */
@property (assign, nonatomic) NSUInteger maxMemoryCountLimit;

/**
 * The maximum length of time to keep an image in the cache, in seconds
 */
@property (assign, nonatomic) NSInteger maxCacheAge;

/**
 * The maximum size of the cache, in bytes.
 */
@property (assign, nonatomic) NSUInteger maxCacheSize;

/**
 * Returns global shared cache instance
 *
 * @return SDImageCache global instance
 */
+ (SDImageCache *)sharedImageCache;

/**
 * Init a new cache store with a specific namespace
 *
 * @param ns The namespace to use for this cache store
 */
- (id)initWithNamespace:(NSString *)ns;

/**
 * Init a new cache store with a specific namespace and directory
 *
 * @param ns        The namespace to use for this cache store
 * @param directory Directory to cache disk images in
 */
- (id)initWithNamespace:(NSString *)ns diskCacheDirectory:(NSString *)directory;

-(NSString *)makeDiskCachePath:(NSString*)fullNamespace;

/**
 * Add a read-only cache path to search for images pre-cached by SDImageCache
 * Useful if you want to bundle pre-loaded images with your app
 *
 * @param path The path to use for this read-only cache path
 */
- (void)addReadOnlyCachePath:(NSString *)path;

/**
 * Store an image into memory and disk cache at the given key.
 *
 * @param image The image to store
 * @param key   The unique image cache key, usually it's image absolute URL
 */
- (void)storeImage:(UIImage *)image forKey:(NSString *)key;

/**
 * Store an image into memory and optionally disk cache at the given key.
 *
 * @param image  The image to store
 * @param key    The unique image cache key, usually it's image absolute URL
 * @param toDisk Store the image to disk cache if YES
 */
- (void)storeImage:(UIImage *)image forKey:(NSString *)key toDisk:(BOOL)toDisk;

/**
 * Store an image into memory and optionally disk cache at the given key.
 *
 * @param image       The image to store
 * @param recalculate BOOL indicates if imageData can be used or a new data should be constructed from the UIImage
 * @param imageData   The image data as returned by the server, this representation will be used for disk storage
 *                    instead of converting the given image object into a storable/compressed image format in order
 *                    to save quality and CPU
 * @param key         The unique image cache key, usually it's image absolute URL
 * @param toDisk      Store the image to disk cache if YES
 */
- (void)storeImage:(UIImage *)image recalculateFromImage:(BOOL)recalculate imageData:(NSData *)imageData forKey:(NSString *)key toDisk:(BOOL)toDisk;

/**
 * Store image NSData into disk cache at the given key.
 *
 * @param imageData The image data to store
 * @param key   The unique image cache key, usually it's image absolute URL
 */
- (void)storeImageDataToDisk:(NSData *)imageData forKey:(NSString *)key;

/**
 * Query the disk cache asynchronously.
 *
 * @param key The unique key used to store the wanted image
 */
- (NSOperation *)queryDiskCacheForKey:(NSString *)key done:(SDWebImageQueryCompletedBlock)doneBlock;

/**
 * Query the memory cache synchronously.
 *
 * @param key The unique key used to store the wanted image
 */
- (UIImage *)imageFromMemoryCacheForKey:(NSString *)key;

/**
 * Query the disk cache synchronously after checking the memory cache.
 *
 * @param key The unique key used to store the wanted image
 */
- (UIImage *)imageFromDiskCacheForKey:(NSString *)key;

/**
 * Remove the image from memory and disk cache asynchronously
 *
 * @param key The unique image cache key
 */
- (void)removeImageForKey:(NSString *)key;


/**
 * Remove the image from memory and disk cache asynchronously
 *
 * @param key             The unique image cache key
 * @param completion      An block that should be executed after the image has been removed (optional)
 */
- (void)removeImageForKey:(NSString *)key withCompletion:(SDWebImageNoParamsBlock)completion;

/**
 * Remove the image from memory and optionally disk cache asynchronously
 *
 * @param key      The unique image cache key
 * @param fromDisk Also remove cache entry from disk if YES
 */
- (void)removeImageForKey:(NSString *)key fromDisk:(BOOL)fromDisk;

/**
 * Remove the image from memory and optionally disk cache asynchronously
 *
 * @param key             The unique image cache key
 * @param fromDisk        Also remove cache entry from disk if YES
 * @param completion      An block that should be executed after the image has been removed (optional)
 */
- (void)removeImageForKey:(NSString *)key fromDisk:(BOOL)fromDisk withCompletion:(SDWebImageNoParamsBlock)completion;

/**
 * Clear all memory cached images
 */
- (void)clearMemory;

/**
 * Clear all disk cached images. Non-blocking method - returns immediately.
 * @param completion    An block that should be executed after cache expiration completes (optional)
 */
- (void)clearDiskOnCompletion:(SDWebImageNoParamsBlock)completion;

/**
 * Clear all disk cached images
 * @see clearDiskOnCompletion:
 */
- (void)clearDisk;

/**
 * Remove all expired cached image from disk. Non-blocking method - returns immediately.
 * @param completionBlock An block that should be executed after cache expiration completes (optional)
 */
- (void)cleanDiskWithCompletionBlock:(SDWebImageNoParamsBlock)completionBlock;

/**
 * Remove all expired cached image from disk
 * @see cleanDiskWithCompletionBlock:
 */
- (void)cleanDisk;

/**
 * Get the size used by the disk cache
 */
- (NSUInteger)getSize;

/**
 * Get the number of images in the disk cache
 */
- (NSUInteger)getDiskCount;

/**
 * Asynchronously calculate the disk cache's size.
 */
- (void)calculateSizeWithCompletionBlock:(SDWebImageCalculateSizeBlock)completionBlock;

/**
 *  Async check if image exists in disk cache already (does not load the image)
 *
 *  @param key             the key describing the url
 *  @param completionBlock the block to be executed when the check is done.
 *  @note the completion block will be always executed on the main queue
 */
- (void)diskImageExistsWithKey:(NSString *)key completion:(SDWebImageCheckCacheCompletionBlock)completionBlock;

/**
 *  Check if image exists in disk cache already (does not load the image)
 *
 *  @param key the key describing the url
 *
 *  @return YES if an image exists for the given key
 */
- (BOOL)diskImageExistsWithKey:(NSString *)key;

/**
 *  Get the cache path for a certain key (needs the cache path root folder)
 *
 *  @param key  the key (can be obtained from url using cacheKeyForURL)
 *  @param path the cache path root folder
 *
 *  @return the cache path
 */
- (NSString *)cachePathForKey:(NSString *)key inPath:(NSString *)path;

/**
 *  Get the default cache path for a certain key
 *
 *  @param key the key (can be obtained from url using cacheKeyForURL)
 *
 *  @return the default cache path
 */
- (NSString *)defaultCachePathForKey:(NSString *)key;

@end
