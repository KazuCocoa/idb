/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#import <Foundation/Foundation.h>
#import <FBControlCore/FBControlCore.h>
#import "FBXCTestDescriptor.h"

NS_ASSUME_NONNULL_BEGIN

/**
 The base class for storage in idb.
 */
@interface FBIDBStorage : NSObject

#pragma mark Properties

/**
 The target that is being stored against.
 */
@property (nonatomic, strong, readonly) id<FBiOSTarget> target;

/**
 The base path of the storage.
 */
@property (nonatomic, strong, readonly) NSURL *basePath;

/**
 The logger to use.
 */
@property (nonatomic, strong, readonly) id<FBControlCoreLogger> logger;

/**
 The queue to use.
 */
@property (nonatomic, strong, readonly) dispatch_queue_t queue;

@end

/**
 Storage for files
 */
@interface FBFileStorage : FBIDBStorage

#pragma mark Public Methods

/**
 Relocates the file into storage.

 @param url the url to relocate.
 @param error an error out for any error that occurs
 @return the path of the relocated file.
 */
- (nullable NSString *)saveFile:(NSURL *)url error:(NSError **)error;

@end

/**
 Base class for bundle storage.
 */
@interface FBBundleStorage : FBIDBStorage


/**
 Checks the bundle is supported on the current target

 @param bundle Bundle to check
 @param error Set if the targets architecture isn't in the set supported by the bundle
 @return YES if the bundle can run on this target, NO otherwise
 */
- (BOOL)checkArchitecture:(FBBundleDescriptor *)bundle error:(NSError **)error;

/**
 Persist the bundle to storage.

 @param bundle the bundle to persist.
 @param error an error out for any error that occurs.
 */
- (nullable NSString *)saveBundle:(FBBundleDescriptor *)bundle error:(NSError **)error;

@end

/**
 Bundle storage for xctest.
 */
@interface FBXCTestBundleStorage : FBBundleStorage

#pragma mark Public Methods

/**
 Stores a test bundle, based on a containing directory.
 This is useful when the test bundle is extracted to a temporary directory, because it came from an archive.

 @param baseDirectory the directory containing the test bundle.
 @param error an error out for any error that occurs.
 @return the bundle id of the installed test, or nil if failed
 */
- (nullable NSString *)saveBundleOrTestRunFromBaseDirectory:(NSURL *)baseDirectory error:(NSError **)error;

/**
 Stores a test bundle, based on the file path of the actual test bundle.
 This is useful when the test bundle is from an existing and local file path, instead of passed in an archive.

 @param filePath the file path of the bundle.
 @param error an error out for any error that occurs.
 @return the bundle id of the installed test, or nil if failed
 */
- (nullable NSString *)saveBundleOrTestRun:(NSURL *)filePath error:(NSError **)error;

/**
 Get descriptors for all installed test bundles and xctestrun files.

 @param error Set if getting this bundle failed
 @return Set of FBXCTestDescriptors of all installed test bundles and xctestrun files
 */
- (nullable NSSet<id<FBXCTestDescriptor>> *)listTestDescriptorsWithError:(NSError **)error;

/**
 Get test descriptor by bundle id.

 @param bundleId Bundle id of test to get
 @return test descriptor of the test
 */
- (nullable id<FBXCTestDescriptor>)testDescriptorWithID:(NSString *)bundleId error:(NSError **)error;

@end

/**
 Bundle storage for applications.
 */
@interface FBApplicationBundleStorage : FBBundleStorage

#pragma mark Public Methods

/**
 The bundle ids of all persisted applications
 */
@property (nonatomic, copy, readonly) NSSet<NSString *> *persistedApplicationBundleIDs;

/**
 A mapping of bundle ids to persisted applications
 */
@property (nonatomic, copy, readonly) NSDictionary<NSString *, FBApplicationBundle *> *persistedApplications;

@end

/**
 Class to manage storing of artifacts for a particular target
 Each kind of stored artifact is placed in a separate directory and managed by a separate class.
 */
@interface FBIDBStorageManager : NSObject

#pragma mark Initializers

/**
 The designated initializer

 @param target Target to store the test bundles in
 @param logger FBControlCoreLogger to use
 @param error an error out for any error that occurs in creating the storage
 @return a FBTeststorageManager instance on success, nil otherwise.
 */
+ (nullable instancetype)managerForTarget:(id<FBiOSTarget>)target logger:(id<FBControlCoreLogger>)logger error:(NSError **)error;

#pragma mark Properties

/**
 The xctest bundle storage
 */
@property (nonatomic, strong, readonly) FBXCTestBundleStorage *xctest;

/**
 The application bundle storage
 */
@property (nonatomic, strong, readonly) FBApplicationBundleStorage *application;

/**
 The dylib storage.
 */
@property (nonatomic, strong, readonly) FBFileStorage *dylib;

/**
 The dSYM storage.
 */
@property (nonatomic, strong, readonly) FBBundleStorage *dsym;

/**
 The Frameworks storage.
 */
@property (nonatomic, strong, readonly) FBBundleStorage *framework;

#pragma mark Public Methods

/**
 Interpolate any replacements

 @param environment the environment to interpolate.
 @return a dictionary with the replacements defined
 */
- (NSDictionary<NSString *, NSString *> *)interpolateEnvironmentReplacements:(NSDictionary<NSString *, NSString *> *)environment;

@end

NS_ASSUME_NONNULL_END
