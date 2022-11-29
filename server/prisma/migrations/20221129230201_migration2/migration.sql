/*
  Warnings:

  - You are about to drop the column `ownerId` on the `Repository` table. All the data in the column will be lost.
  - You are about to drop the `Post` table. If the table is not empty, all the data it contains will be lost.
  - You are about to drop the `Reference` table. If the table is not empty, all the data it contains will be lost.
  - Added the required column `owner` to the `Repository` table without a default value. This is not possible if the table is not empty.

*/
-- DropForeignKey
ALTER TABLE `Repository` DROP FOREIGN KEY `Repository_ownerId_fkey`;

-- AlterTable
ALTER TABLE `Repository` DROP COLUMN `ownerId`,
    ADD COLUMN `owner` VARCHAR(191) NOT NULL;

-- DropTable
DROP TABLE `Post`;

-- DropTable
DROP TABLE `Reference`;
