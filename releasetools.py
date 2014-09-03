# Copyright (C) 2012 The Android Open Source Project
# Copyright (C) 2014 The CyanogenMod Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""Custom OTA Package commands for Sony devices"""

import common
import os

TARGET_DIR = os.getenv('OUT')

def FullOTA_InstallBegin(self):
  fstab = common.OPTIONS.info_dict["fstab"]
  if fstab:
    p = fstab["/data"]
    self.output_zip.write(os.path.join(TARGET_DIR, "fix_data.sh"), "fix_data.sh")
    self.script.AppendExtra('package_extract_file("fix_data.sh", "/tmp/fix_data.sh");')
    self.script.AppendExtra('set_perm(0, 0, 0777, "/tmp/fix_data.sh");')
    self.script.AppendExtra('run_program("/tmp/fix_data.sh");')
    self.script.AppendExtra('sha1_check(read_file("/tmp/fix_data"),"fb360f9c09ac8c5edb2f18be5de4e80ea4c430d0") && format("%s", "%s", "%s", "%s", "%s");' % (p.fs_type, common.PARTITION_TYPES[p.fs_type], p.device, p.length, p.mount_point))
