-- phpMyAdmin SQL Dump
-- version 4.9.5
-- https://www.phpmyadmin.net/
--
-- Host: localhost:3306
-- Generation Time: Nov 25, 2021 at 08:42 AM
-- Server version: 5.7.33-cll-lve
-- PHP Version: 7.3.27

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `bizutage`
--

-- --------------------------------------------------------

--
-- Table structure for table `challenges`
--

CREATE TABLE `challenges` (
  `id` int(11) NOT NULL,
  `value` int(11) DEFAULT '1',
  `question` text NOT NULL,
  `created_at` timestamp NULL DEFAULT NULL,
  `updated_at` timestamp NULL DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

--
-- Dumping data for table `challenges`
--

INSERT INTO `challenges` (`id`, `value`, `question`, `created_at`, `updated_at`) VALUES
(1, 1, 'Dancing (Baby Shark) Challenge\n(https://www.youtube.com/watch?v=XqZsoesa55w)', '2021-11-14 23:41:33', '2021-11-14 23:41:33'),
(2, 1, 'Dancing with 10 person of your choice.\r\n(https://www.youtube.com/watch?v=eYMni0l8g6Y)', '2021-11-14 23:52:07', '2021-11-14 23:52:07'),
(3, 1, 'Dancing with 3 to 5 person\r\n(https://www.youtube.com/watch?v=Dz92QU-40bA)', '2021-11-14 23:56:27', '2021-11-14 23:56:27'),
(4, 1, 'Singing this song\r\n(https://www.youtube.com/watch?v=POwAm86RYnk)', '2021-11-15 00:00:38', '2021-11-15 00:00:38'),
(5, 1, 'Sing this song\r\n(https://www.youtube.com/watch?v=BwRtzDVS84U)', '2021-11-15 00:01:08', '2021-11-15 00:01:08'),
(6, 1, 'Act like you are a mannequin.', '2021-11-15 00:09:01', '2021-11-15 00:09:01'),
(7, 1, 'Imitate Indian Accent', '2021-11-15 00:11:13', '2021-11-15 00:11:13'),
(8, 1, 'Imitate Swaa3daa Accent', '2021-11-15 00:15:46', '2021-11-15 00:15:46'),
(9, 1, 'Give Us your best Zaghrouta', '2021-11-15 01:41:34', '2021-11-15 01:41:34'),
(10, 1, 'Act like a monkey', '2021-11-15 01:42:29', '2021-11-15 01:42:29'),
(11, 1, 'Act like a chicken with sound effect', '2021-11-15 01:42:38', '2021-11-15 01:42:38'),
(12, 1, 'Do the chicken Dance with 5 of your friends for 30 seconds\r\n(https://www.youtube.com/watch?v=dnjMQhKBInM)', '2021-11-15 01:45:28', '2021-11-15 01:45:28'),
(13, 1, 'Pretend you are a drunk for a 30 seconds', '2021-11-15 01:46:26', '2021-11-15 01:46:26'),
(14, 1, 'tell us a joke and make sure that we laugh', '2021-11-15 01:50:29', '2021-11-15 01:50:29'),
(15, 1, 'Sing the national anthem as loud as you can', '2021-11-15 01:52:04', '2021-11-15 01:52:04'),
(16, 1, 'Act like Dora (The Cartoon character)', '2021-11-15 01:53:30', '2021-11-15 01:53:30'),
(18, 1, 'Meow Like a cat with someone of your choice for 30 seconds', '2021-11-15 01:56:22', '2021-11-15 01:56:22'),
(19, 1, 'play the air violin\r\n(https://www.youtube.com/watch?v=jvipPYFebWc)', '2021-11-15 02:02:50', '2021-11-15 02:02:50'),
(20, 1, 'Jump for 10 seconds', '2021-11-15 02:04:45', '2021-11-15 02:04:45'),
(21, 1, 'Dance Macarena with 10 person\r\n(https://youtu.be/SWHS4HsgnUk)', '2021-11-15 02:08:36', '2021-11-15 02:08:36'),
(22, 1, 'say the alphabet backwards', '2021-11-15 02:10:04', '2021-11-15 02:10:04'),
(23, 1, 'Imitate a Chinese accent for 30 seconds', '2021-11-15 02:10:40', '2021-11-15 02:10:40'),
(24, 1, 'Jump on one foot with your hands on your head for 20 seconds', '2021-11-15 02:11:58', '2021-11-15 02:11:58'),
(25, 1, 'Act like an old man or a woman', '2021-11-15 02:14:51', '2021-11-15 02:14:51'),
(26, 1, 'Cry like a baby', '2021-11-15 02:15:55', '2021-11-15 02:15:55'),
(27, 1, 'Make horrible faces till someone laughs', '2021-11-15 02:18:06', '2021-11-15 02:18:06'),
(28, 1, 'listen to your favorite song with headphone and Sing with it Loud as you can', '2021-11-15 02:23:26', '2021-11-15 02:23:26'),
(29, 1, 'Sing this Song\r\n(https://www.youtube.com/watch?v=F2pw97R4160)', '2021-11-16 04:05:50', '2021-11-16 04:05:50'),
(30, 1, 'Sing this song with english lyrics\r\n(https://www.youtube.com/watch?v=16mM1fSM5Gg)', '2021-11-16 04:37:55', '2021-11-16 04:37:55'),
(31, 1, 'made a random speech topic in front of your peers', '2021-11-16 19:27:32', '2021-11-16 19:27:32'),
(32, 1, 'Pick 5 people to play Jmed with and you gonna make them laugh', '2021-11-16 19:33:54', '2021-11-16 19:33:54'),
(33, 1, 'pick someone with long hair and dfer lih cha3ro, if you don\'t know ! ask your peers', '2021-11-16 19:40:25', '2021-11-16 19:40:25'),
(34, 1, 'Scream like a little girl', '2021-11-16 19:43:01', '2021-11-16 19:43:01'),
(35, 1, 'mime : zebra', '2021-11-16 20:09:27', '2021-11-16 20:09:27'),
(36, 1, 'Howl like a wolf', '2021-11-16 20:09:51', '2021-11-16 20:09:51'),
(37, 1, 'Put your clothing on backwards for the rest of the evening', '2021-11-16 20:10:06', '2021-11-16 20:10:06'),
(38, 1, 'make the public laugh by telling jokes r whatever', '2021-11-16 20:22:43', '2021-11-16 20:22:43'),
(39, 1, 'Penguin walk with a chair', '2021-11-16 21:38:05', '2021-11-16 21:38:05');

-- --------------------------------------------------------

--
-- Table structure for table `failed_jobs`
--

CREATE TABLE `failed_jobs` (
  `id` bigint(20) UNSIGNED NOT NULL,
  `uuid` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `connection` text COLLATE utf8mb4_unicode_ci NOT NULL,
  `queue` text COLLATE utf8mb4_unicode_ci NOT NULL,
  `payload` longtext COLLATE utf8mb4_unicode_ci NOT NULL,
  `exception` longtext COLLATE utf8mb4_unicode_ci NOT NULL,
  `failed_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `migrations`
--

CREATE TABLE `migrations` (
  `id` int(10) UNSIGNED NOT NULL,
  `migration` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `batch` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `migrations`
--

INSERT INTO `migrations` (`id`, `migration`, `batch`) VALUES
(1, '2014_10_12_000000_create_users_table', 1),
(2, '2014_10_12_100000_create_password_resets_table', 1),
(3, '2019_08_19_000000_create_failed_jobs_table', 1),
(4, '2019_12_14_000001_create_personal_access_tokens_table', 1);

-- --------------------------------------------------------

--
-- Table structure for table `password_resets`
--

CREATE TABLE `password_resets` (
  `email` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `token` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `created_at` timestamp NULL DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `personal_access_tokens`
--

CREATE TABLE `personal_access_tokens` (
  `id` bigint(20) UNSIGNED NOT NULL,
  `tokenable_type` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `tokenable_id` bigint(20) UNSIGNED NOT NULL,
  `name` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `token` varchar(64) COLLATE utf8mb4_unicode_ci NOT NULL,
  `abilities` text COLLATE utf8mb4_unicode_ci,
  `last_used_at` timestamp NULL DEFAULT NULL,
  `created_at` timestamp NULL DEFAULT NULL,
  `updated_at` timestamp NULL DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `users`
--

CREATE TABLE `users` (
  `id` bigint(20) UNSIGNED NOT NULL,
  `name` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `email` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `email_verified_at` timestamp NULL DEFAULT NULL,
  `password` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `remember_token` varchar(100) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `created_at` timestamp NULL DEFAULT NULL,
  `updated_at` timestamp NULL DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Indexes for dumped tables
--

--
-- Indexes for table `challenges`
--
ALTER TABLE `challenges`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `failed_jobs`
--
ALTER TABLE `failed_jobs`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `failed_jobs_uuid_unique` (`uuid`);

--
-- Indexes for table `migrations`
--
ALTER TABLE `migrations`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `password_resets`
--
ALTER TABLE `password_resets`
  ADD KEY `password_resets_email_index` (`email`);

--
-- Indexes for table `personal_access_tokens`
--
ALTER TABLE `personal_access_tokens`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `personal_access_tokens_token_unique` (`token`),
  ADD KEY `personal_access_tokens_tokenable_type_tokenable_id_index` (`tokenable_type`,`tokenable_id`);

--
-- Indexes for table `users`
--
ALTER TABLE `users`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `users_email_unique` (`email`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `challenges`
--
ALTER TABLE `challenges`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=40;

--
-- AUTO_INCREMENT for table `failed_jobs`
--
ALTER TABLE `failed_jobs`
  MODIFY `id` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `migrations`
--
ALTER TABLE `migrations`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;

--
-- AUTO_INCREMENT for table `personal_access_tokens`
--
ALTER TABLE `personal_access_tokens`
  MODIFY `id` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `users`
--
ALTER TABLE `users`
  MODIFY `id` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
