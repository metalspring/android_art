/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "partial_mark_sweep.h"

#include "heap.h"
#include "large_object_space.h"
#include "partial_mark_sweep.h"
#include "space.h"
#include "thread.h"

namespace art {

PartialMarkSweep::PartialMarkSweep(Heap* heap, bool is_concurrent)
    : MarkSweep(heap, is_concurrent) {
  cumulative_timings_.SetName(GetName());
}

PartialMarkSweep::~PartialMarkSweep() {

}

void PartialMarkSweep::BindBitmaps() {
  MarkSweep::BindBitmaps();

  Spaces& spaces = GetHeap()->GetSpaces();
  WriterMutexLock mu(Thread::Current(), *Locks::heap_bitmap_lock_);
  // For partial GCs we need to bind the bitmap of the zygote space so that all objects in the
  // zygote space are viewed as marked.
  for (Spaces::iterator it = spaces.begin(); it != spaces.end(); ++it) {
    ContinuousSpace* space = *it;
    if (space->GetGcRetentionPolicy() == kGcRetentionPolicyFullCollect) {
      ImmuneSpace(space);
    }
  }
}

}  // namespace art